# Description

![git-select-file](git-select-file.gif)

**tmenu** is a dynamic menu for tty devices, which reads a list of
newline-separated items from stdin.  When the user selects an item and
presses Return, the selected item is printed to stdout.  Entering text
will narrow the list of items to items that contain the entered text.

# Non/Features

- written in POSIX C99
- emacs-like key bindings
- does not use ncurses

# Installation

    make
    sudo make install

`make install` installs to `/usr/local` by default.  If you want to
install tmenu to a different location, set `PREFIX`:

    make install PREFIX=$HOME/local

If you want to change compilation and linking flags, modify
[script/compile](script/compile) and [script/link](script/link).  Having
the options in separate files causes `make` to recompile the program if
options change.

# Options

- `-l LINES`: Set the height of the completion list in lines to LINES.  Defaults to 3.
- `-p PROMPT`: Set the prompt shown to the user to PROMPT.  Defaults to `>>`.

# Key Bindings

- `Return`, `C-j`: Output the currently selected item on stdout and exit.
- `C-n`: Select the next item in the list.
- `C-p`: Select the previous item in the list.
- `C-a`: Move the cursor to the beginning of the line.
- `C-e`: Move the cursor to the end of the line.
- `C-u`: Delete text until the beginning of the line.
- `C-k`: Delete text until the end of the line.
- `C-d`: Delete the character under the cursor.
- `Backspace`: Delete the character before the cursor.
- `C-f`: Move the cursor forward by one character.
- `C-b`: Move the cursor backward by one character.

# Uses

## Switch git branches

Add an git alias with the following command to switch git branches using
`tmenu`:

    git alias br "!git ls-remote -h . | awk '{print(substr($NF, 12))}' | tmenu | xargs git checkout"

Then use `git br` to interactively switch the git branch.

## Switching between background jobs in the shell

Most shells support job control (suspending tasks by pressing `C-z`,
resuming it later using the `fg` command).  The following shell function
overrides the `fg` builtin to interactively select the job to resume.

**Note**: This does not work when dash, or bash in POSIX mode, is used.
In these cases `jobs` is executed in a subshell, always reporting no
jobs. See
[this bug](https://bugs.launchpad.net/ubuntu/+source/dash/+bug/243406).

    fg() {
      command fg %$(jobs | tmenu | xargs -I% expr match % '\[\([[:digit:]]\+\)\]')
    }

## Edit file in current git repository

The following shell command install `git edit` as an alias for selecting
a file from the current git repository and opening it with `$EDITOR`.

    git alias edit '!$EDITOR "$(git ls-files | tmenu)"'

## Managing per-project settings

If you are frequently working on different projects, it can be useful to
maintain different settings for your shell based on the project you are
in.  Add the following to `$HOME/.${SHELL}rc`:

    if [ -e ./.projectile.sh ]; then
        . ./.projectile.sh
    fi

When a new shell is started, it executes the contents of
`.projectile.sh` in the current directory (if such a file exists).

Switching a project can then be done using the following shell function:

    projectile() {
        PROJECTILE=${PROJECTILE:-$HOME/projects}
        (cd $PROJECTILE/$(printf "%s\n" $PROJECTILE/* |
              xargs -L 1 basename |
              tmenu)
         $SHELL)
    }

Set `PROJECTILE` to the directory containing your projects (default:
`$HOME/projects`), then switch to a project using the `projectile`
command.  Place `.projectile.sh` files with the project specific shell
settings into the project directories.  Things that might be interesting
to do:

- `PATH` to contain the `bin` or `script` directory of the current
  project.  This makes it easy to override certain shell commands.

- Use `.projectile.sh` to set the version of the runtime used for the
  project (e.g. using [chruby](https://github.com/postmodern/chruby) or
  [virtualenv](http://virtualenv.org) for Python).

- Change the prompt to highlight that you are working on a specific
  project now.

- Setup a project-specific [tmux](https://github.com/Thomasadam/tmux)
  configuration

# License

Copyright 2014 Dario Hamidi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

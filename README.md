# pwm

pwm is a password manager using [GnuPG](http://www.gnupg.org) to encrypt and
[Git](http://git-scm.com) to store your passwords.

## Usage

    usage: pwm <cmd> [<args>]

    commands:
      del             delete a password
      gen             generate a password
      get             retrieve a password
      help            show this help
      list            list all passwords
      log             print the log
      set             set a password
      version         show version

## Install

To install pwm make sure you have installed the dependencies:

* [GPGme](http://www.gnupg.org/related_software/gpgme)
* [libgit2](http://libgit2.github.com)

When building from the git repo you need
[autoconf](https://www.gnu.org/software/autoconf) in addition.

### Get the Source

Download a release tarball or clone the repo:

    git clone https://github.com/ushis/pwm.git

### Build and Install

Building pwm is as easy as:

    ./configure --prefix=/usr --with-pwmexecdir=/usr/lib/pwm
    make
    make install

When building from the git repo, run autoconf to generate the configure script:

    autoconf -o configure configure.ac

## Hacking pwm

If you want to hack on pwm, check out the
[API documentation](http://ushis.github.io/pwm).

## LICENSE (GPL)

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

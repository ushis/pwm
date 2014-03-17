# pwm

pwm is a password manager using [GnuPG](http://www.gnupg.org) to encrypt and
[Git](http://git-scm.com) to store your passwords.

## Usage

    usage: pwm [<opts>] <cmd> [<args>]

    options:
      -d <path>       database directory
      -h              show this help
      -k <key>        gpg key id
      -v              show version

    commands:
      del             delete a password
      gen             generate a password
      get             retrieve a password
      list            list all passwords
      log             print the log
      note            get, set or del a password note
      set             set a password

pwm creates a bare git repository at ```~/.pwm```. You can change that
behaviour by using the ```-d``` option or setting the environment
variable ```PWM_HOME```

    export PWM_HOME="${HOME}/path/to/my/pwm/db"

pwm uses the first secret GPG key that can be found, do determine the GPG
encryption recipient. You can give pwm a hint by using the ```-k``` option or
setting the environment variable ```PWM_KEY```

    export PWM_KEY='5G532AB7'

## Install

To install pwm make sure you have installed the dependencies:

* [GPGme](http://www.gnupg.org/related_software/gpgme)
* [libgit2](http://libgit2.github.com)

When building from the git repo you need
[autoconf](https://www.gnu.org/software/autoconf) in addition.

If you want to run unit tests you need the
[Check Unit Testing Framework](http://check.sourceforge.net/).

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

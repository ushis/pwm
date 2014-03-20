#!/usr/bin/env sh

if test "${1}" = "libgit2"; then
  cd ~
  git clone https://github.com/libgit2/libgit2.git libgit2

  cd libgit2
  git checkout v0.20.0

  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DTHREADSAFE=ON
  make
  sudo make install
  exit $?
fi

if test "${1}" = "pwm"; then
  autoconf -o configure configure.ac
  ./configure
  make && make test
  exit $?
fi

echo "unknown command: ${1}"
exit 1

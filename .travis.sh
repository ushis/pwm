#!/usr/bin/env sh

cd ~
git clone https://github.com/libgit2/libgit2.git libgit2

cd libgit2
git checkout v0.20.0

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DTHREADSAFE=ON
make && sudo make install

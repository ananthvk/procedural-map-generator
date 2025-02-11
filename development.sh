#!/bin/sh
# For development, set compiler to clang
rm -rf builddir
meson wrap install fmt
CXX=clang++ CC=clang meson setup -Db_sanitize=address -Db_lundef=false --reconfigure builddir
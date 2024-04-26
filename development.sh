#!/bin/sh
# For development, set compiler to clang
mkdir -p subprojects
meson wrap install gtest
CXX=clang++ CC=clang meson setup -Db_sanitize=address -Db_lundef=false -Ddefault_library=shared --reconfigure builddir
cd builddir
meson test -v
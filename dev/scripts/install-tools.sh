#!/bin/bash

set -eux

sudo apt update -y
sudo apt-get install -y \
\
  build-essential       \
  vim                   \
  lldb                  \
  python3               \
  python3-pip           \
  valgrind              \
  tree                  \
  sparse                \
  strace                \
  readline-doc          \
  libreadline-dev       \
  kmod                  \
  dwarves               \
  clang                 \
  clang-format          \
  cmake                 \
  libssl-dev


#!/bin/bash

set -eux

find ../srcs/ -name "*.h" -exec ln -s {} . \;

#!/bin/bash

qemu-system-x86_64 \
  -S -s -serial none \
  -nographic \
  ./build/hello-x86_64-qemu

# pid=$!
# gdb \
#   -ex "target remote localhost:1234" \
#   -ex "set confirm off"
#
# kill -9 $!

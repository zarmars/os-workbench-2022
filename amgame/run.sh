#!/bin/bash

qemu-system-i386 -s -S -drive format=raw,file=$1

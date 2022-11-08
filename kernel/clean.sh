#!/bin/bash

make clean
cd boot && make clean
cd ..
make && make run

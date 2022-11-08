NAME           := kernel
SRCS           := framework/main.c $(shell find -L ./src/ -name "*.c")
INC_PATH       := include/ framework/

export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH    := x86_64-qemu
# export ARCH    := x86-qemu
# export ARCH    := native
endif

include $(AM_HOME)/Makefile
include ../Makefile.lab
image: git
test: git
	@gcc $(shell find -L ./src/ -name "*.c") \
			 $(shell find -L ./test/ -name "*.c") \
			 -Iframework -Itest -Iinclude -DTEST -lpthread \
			 -o build/test
	@build/test 0
	@build/test 1
	@build/test 2
	@build/test 3

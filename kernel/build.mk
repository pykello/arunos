KERNEL_DIR = kernel

OBJS += $(KERNEL_DIR)/console.o
OBJS += $(KERNEL_DIR)/debug.o
OBJS += $(KERNEL_DIR)/init.o
OBJS += $(KERNEL_DIR)/irq.o
OBJS += $(KERNEL_DIR)/kalloc.o
OBJS += $(KERNEL_DIR)/memory.o
OBJS += $(KERNEL_DIR)/proc.o
OBJS += $(KERNEL_DIR)/startup_pagedir.o
OBJS += $(KERNEL_DIR)/syscall.o
OBJS += $(KERNEL_DIR)/system.o

include $(KERNEL_DIR)/lib/build.mk
include $(KERNEL_DIR)/monitor/build.mk

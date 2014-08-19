KERNEL_DIR = kernel

OBJS += $(KERNEL_DIR)/console.o
OBJS += $(KERNEL_DIR)/debug.o
OBJS += $(KERNEL_DIR)/init.o
OBJS += $(KERNEL_DIR)/irq.o
OBJS += $(KERNEL_DIR)/ksyscall.o
OBJS += $(KERNEL_DIR)/system.o

include $(KERNEL_DIR)/memory/build.mk
include $(KERNEL_DIR)/monitor/build.mk
include $(KERNEL_DIR)/proc/build.mk

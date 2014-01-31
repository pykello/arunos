KERNEL_DIR = kernel

OBJS += $(KERNEL_DIR)/console.o
OBJS += $(KERNEL_DIR)/init.o
OBJS += $(KERNEL_DIR)/irq.o

include $(KERNEL_DIR)/monitor/build.mk

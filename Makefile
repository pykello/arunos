OS = arunos

# options
CPU = arm926ej-s

# tools
AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# flags
CFLAGS = -mcpu=$(CPU) -g -I include -marm -ansi -pedantic -Wall -Wextra \
         -msoft-float -fPIC -mapcs-frame -fno-builtin-printf
ASFLAGS = -mcpu=$(CPU) -g -I include
QEMU_FLAGS = -M versatilepb -cpu arm926 -m 128M -nographic

OBJS = kernel/startup.o

include kernel/build.mk
include lib/build.mk

all: $(OS).bin

$(OS).bin: $(OBJS) $(OS).ld
	$(LD) -T $(OS).ld $(OBJS) -o $(OS).elf
	$(OBJCOPY) -O binary $(OS).elf $(OS).bin

qemu: $(OS).bin
	qemu-system-arm $(QEMU_FLAGS) -kernel $(OS).bin

qemu-gdb: $(OS).bin
	qemu-system-arm $(QEMU_FLAGS) -gdb tcp::26000 -S -kernel $(OS).bin

clean:
	rm -f $(OBJS)
	rm -f $(OS).elf $(OS).bin

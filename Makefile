OS = arunos

arch = versatilepb
ifneq ($(arch),versatilepb)
$(error Only arch=versatilepb is supported)
endif

include arch/$(arch)/config.mk

# tools
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

# flags
CFLAGS = -mcpu=$(CPU) -gstabs -I include -I arch/$(arch)/include -marm \
         -std=c99 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf -fno-builtin-strcpy -Wno-overlength-strings \
         -fno-builtin-exit
ASFLAGS = -mcpu=$(CPU) -g -I include -I arch/$(arch)/include
QEMU_FLAGS = $(ARCH_QEMU_FLAGS) -nographic -audio driver=none \
	-drive if=none,id=disk,format=raw,file=disk.img,readonly=on \
	-device virtio-blk-pci,drive=disk,disable-modern=on

all: $(OS).bin disk.img

OBJS = kernel/startup.o

include kernel/build.mk
include lib/build.mk
include arch/$(arch)/build.mk
include user/build.mk

$(OS).bin: $(OBJS) $(OS).ld lib/libarunos.a
	$(LD) -L arch/$(arch) -T $(OS).ld $(OBJS) lib/libarunos.a -o $(OS).elf
	$(OBJCOPY) -O binary $(OS).elf $(OS).bin
	$(OBJDUMP) -D $(OS).elf > $(OS).asm

qemu: all
	qemu-system-arm $(QEMU_FLAGS) -kernel $(OS).bin

qemu-gdb: all
	qemu-system-arm $(QEMU_FLAGS) -gdb tcp::26000 -S -kernel $(OS).bin

clean:
	rm -f $(OBJS) $(EXTRA_CLEAN)
	rm -f $(OS).elf $(OS).bin $(OS).asm

.PHONY: all clean qemu qemu-gdb test
test: all
	python3 tests/fat.py
	python3 tests/boot.py
	python3 tests/regressions.py
	python3 tests/library.py
	python3 tests/allocator.py

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
CFLAGS = -mcpu=$(CPU) -g -I include -I arch/$(arch)/include -marm \
         -std=c99 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf -fno-builtin-strcpy -Wno-overlength-strings \
         -fno-builtin-exit -MMD -MP
ASFLAGS = -MMD -MP -mcpu=$(CPU) -g -I include -I arch/$(arch)/include
QEMU_FLAGS = $(ARCH_QEMU_FLAGS) -nographic -audio driver=none \
	-drive if=none,id=disk,format=raw,file=disk.img,readonly=on \
	-device virtio-blk-pci,drive=disk,disable-modern=on

all: $(OS).bin disk.img

OBJS = kernel/startup.o

include kernel/build.mk
include lib/build.mk
include arch/$(arch)/build.mk
include user/build.mk

%.o: %.c
	@printf '  Compiling %s\n' '$<'
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o: %.S
	@printf '  Assembling %s\n' '$<'
	@$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $<

$(OS).bin: $(OBJS) $(OS).ld arch/$(arch)/linker.ld lib/libarunos.a
	@printf '  Linking %s\n' '$(OS).elf'
	@$(LD) -L arch/$(arch) -T $(OS).ld $(OBJS) lib/libarunos.a -o $(OS).elf
	@printf '  Creating kernel image %s\n' '$@'
	@$(OBJCOPY) -O binary $(OS).elf $(OS).bin
	@printf '  Writing disassembly %s\n' '$(OS).asm'
	@$(OBJDUMP) -D $(OS).elf > $(OS).asm

qemu: all
	@printf '  Starting QEMU (Ctrl-A, then X to exit)\n'
	@qemu-system-arm $(QEMU_FLAGS) -kernel $(OS).bin

qemu-gdb: all
	@printf '  Starting QEMU, waiting for GDB on port 26000\n'
	@qemu-system-arm $(QEMU_FLAGS) -gdb tcp::26000 -S -kernel $(OS).bin

clean:
	@printf '  Removing build artifacts\n'
	@rm -f $(OBJS) $(EXTRA_CLEAN) $(DEPS)
	@rm -f $(OS).elf $(OS).bin $(OS).asm

.PHONY: all clean qemu qemu-gdb test
test: all
	@python3 tests/fat.py
	@python3 tests/boot.py
	@python3 tests/regressions.py
	@python3 tests/library.py
	@python3 tests/allocator.py
	@python3 tests/build.py

DEPS = $(OBJS:.o=.d) $(LIB_OBJS:.o=.d) $(USER_PROGRAMS:%=%.d)
-include $(DEPS)

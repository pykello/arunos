USER_CC = arm-none-eabi-gcc
USER_CFLAGS = -mcpu=$(CPU) -marm -g0 \
         -std=c99 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf -fno-builtin-strcpy -nostdinc -nostdlib \
         -Iinclude

USER_PROGRAMS = user/shell \
		user/hello \
		user/fork_test \
		user/exec_test \
		user/concurrency_test
EXTRA_CLEAN += $(USER_PROGRAMS)

user/%: user/%.c lib/libarunos.a
	$(USER_CC) $(USER_CFLAGS) -c -o $@.o $<
	$(LD) -Ttext=100 $@.o lib/libarunos.a -o $@
	rm -f $@.o

# A small FAT16 superfloppy; numeric filenames preserve the exec ABI.
disk.img: $(USER_PROGRAMS)
	rm -f $@.tmp
	truncate -s 16M $@.tmp
	mkfs.fat -F 16 -s 2 $@.tmp
	@set -e; i=0; for program in $(USER_PROGRAMS); do \
		mcopy -i $@.tmp $$program ::$$i.elf; i=$$((i + 1)); \
	done
	mv $@.tmp $@

EXTRA_CLEAN += disk.img disk.img.tmp

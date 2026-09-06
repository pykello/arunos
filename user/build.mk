USER_CC = arm-none-eabi-gcc
USER_CFLAGS = -mcpu=$(CPU) -marm -g0 \
         -std=c99 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf -fno-builtin-strcpy -nostdinc -nostdlib \
         -Iinclude

OBJS += user/user_programs.o
USER_PROGRAMS = user/shell \
		user/hello \
		user/fork_test \
		user/exec_test \
		user/concurrency_test
EXTRA_CLEAN += $(USER_PROGRAMS) user/base16 user/user_programs.c

user/%: user/%.c lib/libarunos.a
	$(USER_CC) $(USER_CFLAGS) -c -o $@.o $<
	$(LD) -Ttext=100 $@.o lib/libarunos.a -o $@
	rm -f $@.o

user/base16: user/base16.c
	gcc $< -o $@

user/user_programs.c: user/base16 $(USER_PROGRAMS)
	rm -f $@
	echo "const char *user_programs[][1024] = {" >> $@
	for program in $(USER_PROGRAMS); do \
		user/base16 < $$program >> $@; \
	done
	echo "{0}};" >> $@

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

USER_CC = arm-none-eabi-gcc
USER_CFLAGS = -mcpu=$(CPU) -marm -g0 \
         -std=c99 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf -fno-builtin-strcpy -nostdinc -nostdlib \
         -Iinclude

USER_PROGRAMS = user/shell \
		user/hi_test \
		user/fk_test \
		user/ex_test \
		user/co_test
EXTRA_CLEAN += $(USER_PROGRAMS)

user/%: user/%.c lib/libarunos.a
	@printf '  Compiling %s\n' '$<'
	@$(USER_CC) $(USER_CFLAGS) -MMD -MP -MF $@.d -MT $@ -c -o $@.o $<
	@printf '  Linking program %s\n' '$@'
	@$(LD) -Ttext=100 $@.o lib/libarunos.a -o $@
	@rm -f $@.o

# Short filenames are the shell commands; ELF does not need an extension.
disk.img: $(USER_PROGRAMS) user/build.mk
	@printf '  Creating FAT16 filesystem %s\n' '$@'
	@rm -f $@.tmp
	@truncate -s 16M $@.tmp
	@mkfs.fat -F 16 -s 2 $@.tmp
	@set -e; for program in $(USER_PROGRAMS); do \
		printf '  Adding program %s\n' "$$program"; \
		mcopy -i $@.tmp $$program ::$${program##*/}; \
	done
	@mv $@.tmp $@

EXTRA_CLEAN += disk.img disk.img.tmp

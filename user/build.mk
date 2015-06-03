USER_CC = arm-none-eabi-gcc
USER_CFLAGS = -mcpu=arm1176jz-s -marm -g0 \
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

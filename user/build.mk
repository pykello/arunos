USER_CC = arm-none-eabi-gcc
USER_CFLAGS = -mcpu=arm1176jz-s -marm -g0 \
         -std=c99 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf -fno-builtin-strcpy -nostdinc -nostdlib \
         -Wl,-Ttext=100

OBJS += user/user_programs.o
EXTRA_CLEAN += user/hello user/base16 user/user_programs.c

user/hello: user/hello.c
	$(USER_CC)  $(USER_CFLAGS) -o $@ $<

user/base16: user/base16.c
	gcc $< -o $@

user/user_programs.c: user/base16 user/hello 
	rm -f $@
	echo "const char *user_programs[] = {" >> $@
	user/base16 < user/hello >> $@
	echo "};" >> $@


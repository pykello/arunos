LIB_DIR = lib

LIB_OBJS += $(LIB_DIR)/base16.o
LIB_OBJS += $(LIB_DIR)/math.o
LIB_OBJS += $(LIB_DIR)/string.o
LIB_OBJS += $(LIB_DIR)/syscall.o

include $(LIB_DIR)/stdio/build.mk

$(LIB_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -g0 -nostdlib -c -o $@ $<

$(LIB_DIR)/libarunos.a: $(LIB_OBJS)
	$(AR) rT $@ $(LIB_OBJS)

EXTRA_CLEAN += $(LIB_OBJS) $(LIB_DIR)/libarunos.a

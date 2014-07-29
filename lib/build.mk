LIB_DIR = lib

OBJS += $(LIB_DIR)/base16.o
OBJS += $(LIB_DIR)/math.o
OBJS += $(LIB_DIR)/string.o

include $(LIB_DIR)/stdio/build.mk

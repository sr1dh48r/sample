
NAME = uccp_char
EXTRA_CFLAGS += -DUCCP420_HW=1
EXTRA_CFLAGS += -DIMG_DEBUG=0

OBJS = uccp_drv.o     \
       ldr_api.o      \
       ldr_loader.o   \
       uccp_io.o

#obj-m += $(NAME).o
#$(NAME)-objs := $(OBJS)
obj-$(CONFIG_FWLOAD)  += $(OBJS)


KDIR = /lib/modules/$(shell uname -r)/build
TOP := $(PWD)/../../..

NAME = uccp_char
EXTRA_CFLAGS += -DUCCP420_HW=0
EXTRA_CFLAGS += -DIMG_DEBUG=0

OBJS = uccp_drv.o     \
       ldr_api.o      \
       ldr_loader.o   \
       uccp_io.o

obj-m += $(NAME).o
$(NAME)-objs := $(OBJS)

all: 
	@make -C $(KDIR) M=$(PWD) -I $(KDIR) modules
clean:
	@make -C $(KDIR) M=$(PWD) clean

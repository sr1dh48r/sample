DRIV_DIR=../driver/

INCLUDE= -I./ -Ilib -I$(DRIV_DIR)

BUILDFLAGS=-O2 -Wall -Werror $(INCLUDE)

BUILDFLAGS+=-DFILE_DUMP=0 \
	    -DUCCP420_HW=0 

CC=$(CROSS_COMPILE)gcc
CFLAGS=$(BUILDFLAGS)

SRCS=main.c app.c ldr_parser.c

UCCPLD=uccpld

OBJS=$(SRCS:.c=.o)

.PHONY: all clean

all: $(UCCPLD)

$(UCCPLD): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(UCCPLD) $(OBJS)

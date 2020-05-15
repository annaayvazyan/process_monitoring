KERNELDIR ?= /lib/modules/$(shell uname -r)/build

obj-m := proci.o

proci-objs := ./utils.o

PWD := $(shell pwd)

all:

	make -C $(KERNELDIR) M=$(PWD) modules

clean:

	rm *.o *.mod.c *.order *.markers

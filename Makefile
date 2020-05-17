KERNELDIR ?= /lib/modules/$(shell uname -r)/build

obj-m := monitor.o

monitor-objs := ./utils.o ./proci.o



PWD := $(shell pwd)

all:

	make -C $(KERNELDIR) M=$(PWD) modules

clean:

	rm *.o *.mod.c *.order *.markers

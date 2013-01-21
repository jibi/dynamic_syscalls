obj-m += mdynsc.o
mdynsc-objs := loader.o

ifeq ($(CONFIG_X86_32),y)
	mdynsc-objs += arch/x86_32/patcher.o
endif

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

obj-m += adk.o
obj-m += adk_serial.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install
	depmod -a

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

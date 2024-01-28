obj-m += bds_lkm.o
all:
        make   -b -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
clean:
        make   -b  -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

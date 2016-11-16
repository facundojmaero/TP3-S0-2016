obj-m+=charEncryptor.o
obj-m+=charDesencryptor.o
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) userInterface.c -o userInterface
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm userInterface
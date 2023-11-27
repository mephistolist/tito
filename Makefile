CC=gcc
CFLAGS=-march=native -O2 -Wall -Wextra -fPIC --std=c17 -pipe -s
LDFLAGS=-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

all:
	$(CC) -S -masm=intel -c tito.c $(CFLAGS)
	$(AS) tito.s -o tito.o
	$(CC) hidepid.c -o hidepid.so -D_GNU_SOURCE -shared -ldl -w $(CFLAGS)
	$(CC) tito.o -o tito $(CFLAGS)
clean:
	rm -v tito.o tito.s

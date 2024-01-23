CC=gcc
CFLAGS=-march=native -Wall -Wextra -fPIC -pipe
LDFLAGS=-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

all:
	$(CC) -S -masm=intel -c tito.c $(CFLAGS) -static -O2 --std=c17 -s
	$(AS) tito.s -o tito.o
	$(CC) hidden.c -o hidden.so -shared -ldl $(CFLAGS)
	$(CC) tito.o -o tito $(CFLAGS) -static -O2 --std=c17 -s
	upx --best ./tito
clean:
	rm -v tito.o tito.s
                              

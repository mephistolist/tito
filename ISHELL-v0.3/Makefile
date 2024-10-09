CC = gcc
CFLAGS1 = -march=native -O2 -Wall -Wextra -fPIC -pipe -s
CFLAGS2 = -O2 -Wall -lsocket
STRIP = strip

default:
	@echo "-------------------------------"
	@echo "Make with the OS from the list:"
	@echo ""
	@echo "1.) linux"
	@echo "2.) bsd"
	@echo "3.) solaris"
	@echo ""
	@echo "ex: make bsd"
	@echo "-------------------------------"

clean:
	/bin/rm -f ish ishd

linux:	clean cc1 fin

bsd:	clean cc1 fin

solaris:	clean cc2 fin

cc1:
	$(CC) $(CFLAGS1) -o ish ish.c ish_main.c
	$(CC) $(CFLAGS1) -o ishd ishd.c ish_main.c ish_open.c

cc2:
	$(CC) $(CFLAGS2) -o ish ish.c ish_main.c
	$(CC) $(CFLAGS2) -o ishd ishd.c ish_main.c ish_open.c

fin:
	$(STRIP) ish
	$(STRIP) ishd

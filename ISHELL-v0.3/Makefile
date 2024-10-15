CC = cc
CFLAGS1 = -march=native -O2 -Wall -Wextra -fPIC -pipe -s
CFLAGS2 = -O2 -Wall -lsocket
STRIP = strip

# Default target to display help
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

# Clean the build files
clean:
	/bin/rm -f ish ishd

# Build for Linux
linux:	
	$(MAKE) clean
	$(MAKE) CC=gcc cc1

# Build for BSD
bsd:	
	$(MAKE) clean
	$(MAKE) CC=cc cc1

# Build for Solaris
solaris:
	$(MAKE) clean
	$(MAKE) CC=cc cc2
	$(STRIP) ish
	$(STRIP) ishd

# Compile with CFLAGS1
cc1:
	$(CC) $(CFLAGS1) -o ish ish.c ish_main.c
	$(CC) $(CFLAGS1) -o ishd ishd.c ish_main.c ish_open.c

# Compile with CFLAGS2 (for Solaris)
cc2:
	$(CC) $(CFLAGS2) -o ish ish.c ish_main.c
	$(CC) $(CFLAGS2) -o ishd ishd.c ish_main.c ish_open.c

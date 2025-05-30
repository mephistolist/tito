CC = cc
CFLAGS = -fPIC -shared -Oz -fno-stack-protector -fno-asynchronous-unwind-tables \
         -fno-ident -ffunction-sections -fdata-sections -falign-functions=1 \
         -falign-loops=1 -s -Wa,--no-pad-sections -march=native -pipe
LDFLAGS = -ldl -nostartfiles -Wl,-z,norelro -Wl,-O1 -Wl,--build-id=none \
          -Wl,-z,separate-code -s

TARGET = libhide.so
SRC = libhide.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

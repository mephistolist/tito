#!/usr/bin/python3
import mmap
import ctypes

# Shellcode
shellcode = (b"\x48\x31\xc9\x48\x81\xe9\xf7\xff\xff\xff\x48\x8d\x05\xef\xff\xff\xff\x48\xbb\xa6\xa3\x1a\xd4\xa5\x07\x96\xe4\x48\x31\x58\x27\x48\x2d\xf8\xff\xff\xff\xe2\xf4\xee\x1b\x35\xb6\xcc\x69\xb9\x97\xce\xa3\x83\x84\xf1\x58\xc4\x82\xce\x8e\x79\x80\xfb\x55\x7e\xf9\xa6\xa3\x1a\xfb\xcd\x68\xfb\x81\x89\xd3\x72\xe7\x96\x75\xb9\xad\xf5\xeb\x5f\x98\xe9\x2a\xe0\xd4\x88\x91\x35\xbd\xd6\x6f\xf2\xe4\xf0\xf4\x4e\x8a\xcf\x3c\xce\xeb\xa3\xa3\x1a\xd4\xa5\x07\x96\xe4")

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

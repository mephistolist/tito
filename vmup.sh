#!/bin/sh

wget 'https://mfsbsd.vx.sk/files/iso/14/amd64/mfsbsd-14.0-RELEASE-amd64.iso' -O /tmp/
sh -c "qemu-system-x86_64 -boot d -cdrom /tmp/mfsbsd-14.0-RELEASE-amd64.iso -m 4096 -smp 8 -device e1000,netdev=net0 -netdev user,id=net0,hostfwd=tcp::5555-:22"

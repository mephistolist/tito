/* ISHELL: main
 * by: Peter Kieltyka (elux)
 * http://peter.eluks.com / peter@eluks.com
 * Updates from 0.2 by Mephistolist */
#include "ishell.h"

struct  ish_track ish_info = {
                1515,   /* ish id                       */
                0,      /* icmp type to send packet as  */
                512,    /* packetsize                   */
		            0       /* seq				*/
};

struct  ish_hdr sendhdr;

int ish_send(int sockfd, char *send_buf, struct sockaddr *sin, socklen_t sinlen) {
    int     datalen = sizeof(struct icmp) + sizeof(struct ish_hdr) + strlen(send_buf);
    char    datagram[datalen];
    char    *ish  = datagram + sizeof(struct icmp);
    char    *data = datagram + sizeof(struct icmp) + sizeof(struct ish_hdr);
    struct  icmp *icmph = (struct icmp *)datagram;

    memset(datagram, 0, sizeof(datagram));
    memcpy(data, send_buf, strlen(send_buf));
    memcpy(ish, &sendhdr, sizeof(struct ish_hdr));

    icmph->icmp_type  = ish_info.type;
    icmph->icmp_code  = 0;
    icmph->icmp_id    = ish_info.id;
    icmph->icmp_seq   = ish_info.seq;
    icmph->icmp_cksum = 0;
    icmph->icmp_cksum = in_cksum((u_short *)datagram, datalen);

    if(sendto(sockfd, datagram, datalen, 0, sin, sinlen) < 0)
        return(-1);

    return(0);
}

int ish_recv(int sockfd, char *recv_buf, struct sockaddr *sin, socklen_t sinlen) {
    int     datalen = ish_info.packetsize + sizeof(struct ip) + 
                      sizeof(struct icmp) + sizeof(struct ish_hdr);
    int     n;
    char    datagram[datalen];
    struct  icmp    *icmph   = (struct icmp *)(datagram + sizeof(struct ip));
    struct  ish_hdr *recvhdr = (struct ish_hdr *)(datagram + sizeof(struct ip) + sizeof(struct icmp));
    char    *data            = datagram + sizeof(struct ip) + sizeof(struct icmp) + sizeof(struct ish_hdr);

    memset(datagram, 0, sizeof(datagram));
    n = recvfrom(sockfd, datagram, sizeof(datagram), 0, sin, &sinlen);
    if (n < 0) {
    	// Handle error
	    perror("recvfrom failed");
	    return -1;
    }

    if(icmph->icmp_id != ish_info.id)
        return(-1);

    if(recv_buf != NULL) {
	      memset(recv_buf, 0, ish_info.packetsize);
        memcpy(recv_buf, data, strlen(data));
    }

    if(recvhdr->cntrl & CNTRL_CEXIT)
        return(CNTRL_CEXIT);
    else if(recvhdr->cntrl & CNTRL_CPOUT)
        return(CNTRL_CPOUT);

    return(0);
}

void error_msg(void) {
    fprintf(stderr, "Error: %s.\n", strerror(errno));
    exit(-1);
}

/* This function is taken from the public domain version of Ping */
unsigned short in_cksum(unsigned short *addr, int len) {
    int             nleft = len;
    int             sum = 0;
    unsigned short  *w = addr;
    unsigned short  answer = 0;

    /*  Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits. */
    while(nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    /* 4mop up an odd byte, if necessary */
    if(nleft == 1) {
        *(unsigned char *)(&answer) = *(unsigned char *)w ;
        sum += answer;
    }

    /* 4add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);                 /* add carry */
    answer = ~sum;                      /* truncate to 16 bits */
    return(answer);
}

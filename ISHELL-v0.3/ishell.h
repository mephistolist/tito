/* ISHELL: header
 * by: Peter Kieltyka (elux)
 * http://peter.eluks.com / peter@eluks.com */
#ifndef __i_shell_h
#define __i_shell_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <fcntl.h>
#include <netdb.h>

struct ish_hdr {
    uint16_t   cntrl;          /* ish flag control                  */
    uint32_t   ts;             /* timestamp when sent               */
};

struct ish_track {
    uint16_t   id;             /* ish id                            */
    uint8_t    type;           /* icmp type to send packet as       */
    uint16_t   packetsize;     /* packetsize of data                */
    uint32_t   seq;            /* sequence number tracking          */
};

#define VERSION         "0.2"   /* ishell version                    */
#ifndef MAX
#define MAX(a,b)        ((a)>(b)?(a):(b))
#endif
#define CNTRL_CEXIT     1       /* client exited                     */
#define CNTRL_CPOUT     2       /* client packet out                 */


/* function prototypes */
int     popen2(const char *);
int     pclose2(int);
int     ish_send(int, char *, struct sockaddr *, socklen_t);
int     ish_recv(int, char *, struct sockaddr *, socklen_t);
void    error_msg(void);
u_short in_cksum(u_short *, int);

/* global variables    */
extern  struct ish_track ish_info;
extern  struct ish_hdr sendhdr;

#endif

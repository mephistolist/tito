#include <linux/bpf.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/if_ether.h> // Ethernet header definitions
#include <bpf/bpf_helpers.h>

// Define the port to hide (e.g., port 8080)
#define HIDE_PORT 12345

SEC("tc") // Use the correct section for traffic control
int hide_port(struct __sk_buff *skb) {
    // Calculate the Ethernet header offset
    void *data = (void *)(long)skb->data;
    void *data_end = (void *)(long)skb->data_end;
    struct ethhdr *eth = data;

    // Ensure the Ethernet header is within the bounds of the packet
    if ((void *)(eth + 1) > data_end) {
        return -1; // Packet is malformed, allow it
    }

    // Check if it's an IPv4 packet (EtherType = 0x0800)
    if (eth->h_proto == __constant_htons(ETH_P_IP)) {
        struct iphdr *iph = (struct iphdr *)(eth + 1);

        // Ensure the IP header is within the bounds of the packet
        if ((void *)(iph + 1) > data_end) {
            return -1; // Packet is malformed, allow it
        }

        // Check if it's a TCP packet
        if (iph->protocol == IPPROTO_TCP) {
            struct tcphdr *tcph = (struct tcphdr *)((void *)iph + (iph->ihl * 4));

            // Ensure the TCP header is within the bounds of the packet
            if ((void *)(tcph + 1) > data_end) {
                return -1; // Packet is malformed, allow it
            }

            // If the destination port matches HIDE_PORT, drop the packet
            if (tcph->dest == __constant_htons(HIDE_PORT)) {
                return 0; // Drop the packet
            }
        }
    }

    return -1; // Allow the packet
}

char _license[] SEC("license") = "GPL";

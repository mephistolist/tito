#ifndef _bds_structs_H_
#define _bds_structs_H_
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
    #define KPROBE_LOOKUP 1
    #include <linux/kprobes.h>
    static struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name",
    };
#endif
	
struct linux_dirent {
	unsigned long   d_ino;
	unsigned long   d_off;
	unsigned short  d_reclen;
	char            d_name[1];
};

#endif

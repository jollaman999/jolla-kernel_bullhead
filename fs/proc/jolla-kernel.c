#include <linux/fs.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <generated/compile.h>

static int jolla_kernel_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "{\"kernel-name\": \"jolla-kernel_bullhead_II\","
			"\"version\": \"v8.1\","
			"\"buildtime\": \"%s\"}\n", JOLLA_KERNEL_TIMESTAMP);
	return 0;
}

static int jolla_kernel_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, jolla_kernel_proc_show, NULL);
}

static const struct file_operations jolla_kernel_proc_fops = {
	.open		= jolla_kernel_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_jolla_kernel_init(void)
{
	proc_create("jolla-kernel", 0, NULL, &jolla_kernel_proc_fops);
	return 0;
}
module_init(proc_jolla_kernel_init);

#include <dru_sysctl.h>


#ifdef CONFIG_PROC_FS


#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *dru_proc_dir	= NULL;


#if defined(CONFIG_DRU_UDP_FILTER_STAT)
static struct proc_dir_entry *dru_proc_ufstat	= NULL;
static int udp_filter_stat_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "omitted  = %llu\naccepted = %llu\ndropped  = %llu\n",
	           udp_filter_stat.omitted, udp_filter_stat.accepted, udp_filter_stat.dropped);
	return 0;
}
static int udp_filter_stat_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, udp_filter_stat_proc_show, NULL);
}
static const struct file_operations udp_filter_stat_proc_fops = {
	.open		= udp_filter_stat_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif


#if defined(CONFIG_DRU_LOAD_FILTER_STAT)
static struct proc_dir_entry *dru_proc_ldstat	= NULL;
static int load_filter_stat_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "accepted = %llu\ndropped  = %llu\n",
	           load_filter_stat.accepted, load_filter_stat.dropped);
	return 0;
}
static int load_filter_stat_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, load_filter_stat_proc_show, NULL);
}
static const struct file_operations load_filter_stat_proc_fops = {
	.open		= load_filter_stat_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

#if defined(CONFIG_DRU_IDLE_FILTER_STAT)
static struct proc_dir_entry *dru_proc_idstat	= NULL;
static int idle_filter_stat_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "accepted = %llu\ndropped  = %llu\n",
	           idle_filter_stat.accepted, idle_filter_stat.dropped);
	return 0;
}
static int idle_filter_stat_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, idle_filter_stat_proc_show, NULL);
}
static const struct file_operations idle_filter_stat_proc_fops = {
	.open		= idle_filter_stat_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

static struct proc_dir_entry *dru_proc_sysstat	= NULL;
static int sysstat_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "idle = %u %u%%\n",
	           (unsigned)dru_sysstat.idle.diff,
	           dru_sysstat.idle.percents);
	return 0;
}
static int sysstat_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, sysstat_proc_show, NULL);
}
static const struct file_operations sysstat_proc_fops = {
	.open		= sysstat_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};


int dru_proc_init(void)
{
	dru_proc_dir = proc_mkdir("dru", NULL);

	if (!dru_proc_dir) {
		printk(KERN_ERR "cannot create /proc/dru. procfs will not supported\n");
		return 0;
	}

#if defined(CONFIG_DRU_UDP_FILTER_STAT)
	dru_proc_ufstat = proc_create("udp_filter_stat", 0444, dru_proc_dir, &udp_filter_stat_proc_fops);

	if (!dru_proc_ufstat)
		printk(KERN_ERR "cannot create /proc/dru/udp_filter_stat\n");

#endif
#if defined(CONFIG_DRU_LOAD_FILTER_STAT)
	dru_proc_ldstat = proc_create("load_filter_stat", 0444, dru_proc_dir, &load_filter_stat_proc_fops);

	if (!dru_proc_ldstat)
		printk(KERN_ERR "cannot create /proc/dru/load_filter_stat\n");

#endif
#if defined(CONFIG_DRU_IDLE_FILTER_STAT)
	dru_proc_idstat = proc_create("idle_filter_stat", 0444, dru_proc_dir, &idle_filter_stat_proc_fops);

	if (!dru_proc_idstat)
		printk(KERN_ERR "cannot create /proc/dru/idle_filter_stat\n");

#endif

	dru_proc_sysstat = proc_create("sysstat", 0444, dru_proc_dir, &sysstat_proc_fops);

	if (!dru_proc_sysstat)
		printk(KERN_ERR "cannot create /proc/dru/sysstat\n");

	return 0;
}

void dru_proc_exit(void)
{
	if (dru_proc_sysstat)	proc_remove(dru_proc_sysstat);

#if defined(CONFIG_DRU_UDP_FILTER_STAT)

	if (dru_proc_ufstat)	proc_remove(dru_proc_ufstat);

#endif
#if defined(CONFIG_DRU_LOAD_FILTER_STAT)

	if (dru_proc_ldstat)	proc_remove(dru_proc_ldstat);

#endif
#if defined(CONFIG_DRU_IDLE_FILTER_STAT)

	if (dru_proc_idstat)	proc_remove(dru_proc_idstat);

#endif

	if (dru_proc_dir)	proc_remove(dru_proc_dir);
}

#else	/* CONFIG_PROC_FS */
int  dru_proc_init(void)
{
	return 0;
}
void dru_proc_exit(void)
{
	return;
}
#endif

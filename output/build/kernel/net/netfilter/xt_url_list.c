/* Kernel module to match HTTP URL's. */

/* Copyright (C) 2015 farisey <anikulin@dlink.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

//#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>	/* kmalloc/kfree */
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/fs.h>

//#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25)
#include <linux/semaphore.h>
//#else
//#include <asm/semaphore.h>
//#endif
#include <linux/uaccess.h>

#include <net/netfilter/xt_url.h>


static struct list_head *url_lists __read_mostly = NULL;
static int url_lists_num = 0;
static struct hlist_head all_hlist __read_mostly = HLIST_HEAD_INIT;

struct xt_url {
	struct list_head list;		/* head in indexed list */
	struct hlist_node hnode_in_all;	/* node in common hlist */
	struct rcu_head rcu;		/* for freeing */
	unsigned list_ix;
	char *host;
	char *path;
	__u16 host_len, path_len;
};

static struct xt_url *xt_url_alloc(unsigned list_ix, const char *host, const char *path)
{
	struct xt_url *url = kmalloc(sizeof(struct xt_url), GFP_KERNEL);
	char *x;

	if (unlikely(!url)) return NULL;

	url->host_len = strlen(host);//host ? strlen(host) : 0;
	url->host = kmalloc(url->host_len + 1, GFP_KERNEL);

	if (unlikely(!url->host)) goto host_failed;

	url->path_len = strlen(path);//path ? strlen(path) : 0;
	url->path = kmalloc(url->path_len + 1, GFP_KERNEL);

	if (unlikely(!url->path)) goto path_failed;

	INIT_LIST_HEAD(&url->list);
	INIT_HLIST_NODE(&url->hnode_in_all);
	//no need: http://lkml.iu.edu/hypermail/linux/kernel/1002.2/02070.html  INIT_RCU_HEAD(&url->rcu);//?needed?
	url->list_ix = list_ix;
	/* fast strcpy */
	memcpy(url->host, host, url->host_len + 1);
	memcpy(url->path, path, url->path_len + 1);

	/* to lower */
	for (x = url->host; *x; x++) *x = tolower(*x);

	for (x = url->path; *x; x++) *x = tolower(*x);

	return url;
path_failed:
	kfree(url->host);
host_failed:
	kfree(url);
	return NULL;
}

static void xt_url_free_rcu(struct rcu_head *rcu_head)
{
	struct xt_url *url = container_of(rcu_head, struct xt_url, rcu);
	kfree(url->path);
	kfree(url->host);
	kfree(url);
}

static void
xt_url_flush(int idx)
{
	struct xt_url *url;
	/* Do not use the _rcu iterator here, since this is the only
	 * deletion routine. */
	list_for_each_entry(url, url_lists + idx, list) {
		list_del_rcu(&url->list);
		hlist_del_rcu(&url->hnode_in_all);
		call_rcu(&url->rcu, xt_url_free_rcu);
	}
}

static void
xt_url_flush_all(void)
{
	int i;

	for (i = 0; i < url_lists_num; i++)
		xt_url_flush(i);
}

static __always_inline int
my_starts_with_nocase(const char *x, const char *y, size_t len)
{
	while (len--)
		if (tolower(*x++) != *y++) return 0;

	return 1;
}

/* @s is not null-terminated and
 * contains any case
 * @p - is lowercase null-terminated
 * searching is case insensitive
 * !! slen >= plen !!
 */
static __always_inline const char *
strnNcasestr(const char *s, size_t slen,
             const char *p, size_t plen)
{
	/* number of cycles
	 * check: if slen == plen then
	 * need to compare once
	 */
	slen = slen - plen + 1;

	for (; slen; slen--, s++)
		if (my_starts_with_nocase(s, p, plen))
			return s;

	return NULL;
}

bool
xt_url_match_found(__u8 list_ix,
                   const char *host, __u16 host_len,
                   const char *path, __u16 path_len)
{
	struct xt_url *url;

	rcu_read_lock();
	list_for_each_entry_rcu(url, url_lists + list_ix, list) {
		/* exclude all non-matching */

		/* no host => all matched */
		if (url->host_len) {
			/* given is shorter than pattern */
			if (host_len < url->host_len) continue;

			/* no substring found */
			if (!strnNcasestr(host, host_len,
			                  url->host, url->host_len))
				continue;
		}

		/* no path => all matched */
		if (url->path_len) {
			if (path_len < url->path_len) continue;

			if (url->path[0] == '/') {
				/* not "starts with" */
				if (!my_starts_with_nocase(path, url->path,
				                           url->path_len))
					continue;
			} else {
				/* no pattern found in supplied path */
				if (!strnNcasestr(path, path_len,
				                  url->path, url->path_len))
					continue;
			}
		}

		rcu_read_unlock();
		return true; /* matched ! */
	}
	rcu_read_unlock();
	return false;
}

/*   /proc interface   */

#define XT_URL_PROC_NAME	"xt_url"
static struct proc_dir_entry *xt_url_proc = NULL;

struct xt_url_seq_priv {
	/* WARNING Is a lock needed between clones that shared fd ? */
	int eof;			/* next() have got the end  */
};
//static DEFINE_SEMAPHORE(xt_url_proc_sema);
/* data for seq, are protected by sema above */
//static struct hlist_head *xt_url_proc_cur_list = NULL;
//static int xt_url_proc_cur_ix = 0;

static int xt_url_seq_show(struct seq_file *f, void *v)
{
	struct xt_url *url = hlist_entry(v, struct xt_url, hnode_in_all);

	if (v == SEQ_START_TOKEN)
		seq_printf(f , "# list:\thost\tpath\n");
	else
		seq_printf(f , "%d:\t%s\t%s\n",
		           url->list_ix,
		           url->host[0] ? url->host : "*",
		           url->path[0] ? url->path : "*"
		          );

	return 0;
}

static void *xt_url_seq_start(struct seq_file *f, loff_t *ppos)
{
	struct xt_url_seq_priv *priv = (struct xt_url_seq_priv *)f->private;

	//down(&xt_url_proc_sema);
	rcu_read_lock();

	/*
	 * little optimization
	 * common case: seq_read() have read all
	 * data and runs the second time with saved pos
	 * (i guess to check the rest). Do not find acquired
	 * offset, because this is useless.
	 */
	if (*ppos && priv->eof) return NULL;

	priv->eof = 0;
	return seq_hlist_start_head_rcu(&all_hlist, *ppos);
}

static void *xt_url_seq_next(struct seq_file *f, void *v, loff_t *ppos)
{
	struct xt_url_seq_priv *priv = (struct xt_url_seq_priv *)f->private;
	struct hlist_node *n = seq_hlist_next_rcu(v, &all_hlist, ppos);

	if (!n)
		priv->eof = 1;

	return n;
}

static void xt_url_seq_stop(struct seq_file *f, void *v)
{
	rcu_read_unlock();
	//up(&xt_url_proc_sema);
}

static const struct seq_operations xt_url_seq_ops = {
	.start	= xt_url_seq_start,
	.next	= xt_url_seq_next,
	.stop	= xt_url_seq_stop,
	.show	= xt_url_seq_show,
};

static int xt_url_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_private(file, &xt_url_seq_ops, sizeof(struct xt_url_seq_priv));
}

static inline void xt_url_skip_spaces(char **ptr)
{
	while (**ptr == ' ') (*ptr)++;
}
/*
 * "+N: URL" , where N - index of list
 * "+N: HOST PATH" host and path can be single '*'
 * "+N: /PATH"
 */
static ssize_t xt_url_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	char *urlstr;
	ssize_t res = 0; /* avoid gcc warning */
	unsigned ix;
	char *host, *path;
	struct xt_url *url;

	if (count >= 1024 - 1) /* -1 for possible memmove below */
		return -EMSGSIZE;

	/* partial write is not supported */
	if (*offset)
		return -ESPIPE;

	if (!access_ok(VERIFY_READ, buf, count))
		return -EFAULT;

	/* +1 - terminator
	 * +1 - possible memmove below
	 */
	urlstr = kmalloc(count + 1 + 1, GFP_KERNEL);

	if (unlikely(!urlstr))
		return -ENOMEM;

	if (copy_from_user(urlstr, buf, count)) {
		res = -EFAULT;
		goto err_exit;
	}

	urlstr[count] = '\0';

	/* prepare */
	for (host = urlstr; *host; host++) {
		if (*host == '\t') *host = ' ';
		else if ( (*host == '\n') || (*host == '\r') ) {
			*host = '\0';
			break; /* new EOL */
		}
	}

	res = -EINVAL;

	/* flash all */
	if (!strncmp(urlstr, "flush", 5)) {
		xt_url_flush_all();
		goto ok_exit;
	}

	/* flash one */
	if (urlstr[0] == '-') {
		char c;

		if (sscanf(urlstr, "-%u:%c", &ix, &c) != 2)
			goto err_exit;

		if (c != '*')
			goto err_exit;

		if (ix >= url_lists_num) {
			res = -ERANGE;
			goto err_exit;
		}

		xt_url_flush(ix);
		goto ok_exit;
	}

	/* add URL */
	if (sscanf(urlstr, "+%u:", &ix) != 1)
		goto err_exit;

	if (ix >= url_lists_num) {
		res = -ERANGE;
		goto err_exit;
	}

	host = strchr(urlstr, ' ');

	if (!host)
		goto err_exit;

	xt_url_skip_spaces(&host);

	if (!strncmp(host, "http://", 7))
		host += 7;

	/* attention: host can be "\0",
	 * do not search from host+1
	 */
	if (host[0] == '/') {
		/* "/PATH" */
		path = host;
		host = NULL;
	} else if ( (path = strchr(host, ' ')) ) {
		/* "HOST PATH" */
		if (path) {
			*path++ = '\0';
			xt_url_skip_spaces(&path);
		}
	} else if ( (path = strchr(host, '/')) ) {
		/* "HOST/PATH" */
		memmove(path + 1, path, strlen(path) + 1);
		*path++ = '\0';
	} else
		path = NULL; /* "HOST" only */

	/* check */
	if (host) {
		if (strchr(host, ' ')) goto err_exit;

		if (host[0] == '*') host = NULL;	/* any */
	}

	if (path) {
		if (strchr(path, ' ')) goto err_exit;

		if (path[0] == '*') path = NULL;	/* any */
	}

	url = xt_url_alloc(ix, host ? host : "", path ? path : "");

	if (unlikely(!url)) {
		res = -ENOMEM;
		goto err_exit;
	}

	list_add_tail_rcu(&url->list, url_lists + ix);
	hlist_add_head_rcu(&url->hnode_in_all, &all_hlist);

ok_exit:
	res = count;
	*offset += count;
err_exit:
	kfree(urlstr);
	return res;
}

static const struct file_operations xt_url_proc_ops = {
	.owner	 = THIS_MODULE,
	.open	 = xt_url_seq_open,
	.read	 = seq_read,
	.write	 = xt_url_write,
	.llseek	 = seq_lseek,
	.release = seq_release_private,
};

/*   main   */

int
xt_url_alloc_lists(int count)
{
	int i;

	url_lists = kmalloc(count * sizeof(struct list_head), GFP_KERNEL);

	if (unlikely(!url_lists))
		return -ENOMEM;

	url_lists_num = count;

	for (i = 0; i < count; i++)
		INIT_LIST_HEAD(url_lists + i);

	xt_url_proc = proc_create(XT_URL_PROC_NAME, S_IRUSR | S_IWUSR | S_IRGRP, NULL, &xt_url_proc_ops);

	if (!xt_url_proc) {
		printk(KERN_ERR "cannot create /proc/%s\n", XT_URL_PROC_NAME);
		kfree(url_lists);
		return -ENOMEM;
	}

	return 0;
}


/* kfree() is not protected by RCU
 * call this func after matching deregistered
 * (no readers possible)
 */
void
xt_url_free_lists(void)
{
	proc_remove(xt_url_proc);
	xt_url_flush_all();
	rcu_barrier();/* wait all call_rcu() */
	kfree(url_lists);
}

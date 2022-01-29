#include <linux/rtnetlink.h>
#include <linux/rcupdate.h>
#include <linux/rculist.h>
#include "internal.h"


struct mpls_substack {
	struct list_head list;
	u32 input_label;
	struct list_head handlers;
};
struct mpls_subhandler {
	struct list_head list;

	/* id of the subhandler.
	 * struct net_device* for VPLS */
	void *hnd_arg;

	/* called from mpls_forward() under RCU
	 */
	mpls_substack_recv hnd_recv;
};


static struct mpls_substack *find_label_rcu(struct net *net, u32 label)
{
	struct mpls_substack *st;
	list_for_each_entry_rcu(st, &net->mpls.substacks, list) {
		if (st->input_label == label)
			return st;
	}

	return NULL;
}

static struct mpls_subhandler *find_handler_rcu(struct mpls_substack *st,
						void *hnd_arg)
{
	struct mpls_subhandler *hnd;
	list_for_each_entry_rcu(hnd, &st->handlers, list) {
		if (hnd->hnd_arg == hnd_arg)
			return hnd;
	}

	return NULL;
}


void mpls_substack_init(struct net *net)
{
	INIT_LIST_HEAD(&net->mpls.substacks);
}

void mpls_substack_free(struct net *net)
{
	/* under RTNL,
	 * this is for net_exit(), so
	 * 1. All should be freed already by
	 *    netdevs unregister.
	 * 2. No RCU synchronization is needed.
	 */
	BUG_ON(!list_empty(&net->mpls.substacks));
}



int mpls_substack_rcv(struct net *net, struct sk_buff *skb,
		      struct mpls_entry_decoded *dec)
{
	struct mpls_substack *st;
	struct mpls_subhandler *hnd;
	int hits = 0;

	st = find_label_rcu(net, dec->label);

	if (!st)
		return NOTIFY_DONE;

	list_for_each_entry_rcu(hnd, &st->handlers, list) {
		int err = hnd->hnd_recv(hnd->hnd_arg, skb, net->loopback_dev,
					dec);

		if (err & NOTIFY_STOP_MASK)
			return err;

		if (err == NOTIFY_OK)
			hits++;
	}

	return hits ? NOTIFY_OK : NOTIFY_DONE;
}

int mpls_substack_add(struct net *net, u32 input_label,
		      void *hnd_arg, mpls_substack_recv recv/*,
		      struct netlink_ext_ack *extack*/)
{
	struct mpls_substack *st;
	struct mpls_subhandler *hnd;
	bool st_is_new = false;
	int ret = 0;

	ASSERT_RTNL();

	rcu_read_lock();
	st = find_label_rcu(net, input_label);
	if (!st) {
		rcu_read_unlock();
		st = kmalloc(sizeof(*st), GFP_KERNEL);
		if (unlikely(!st)) {
			////NL_SET_ERR_MSG(extack, "No mem for substack label");
			ret = -ENOMEM;
		}
		st_is_new = true;
		st->input_label = input_label;
		INIT_LIST_HEAD(&st->handlers);
	} else {
		hnd = find_handler_rcu(st, hnd_arg);
		rcu_read_unlock();
		if (unlikely(hnd)) {
			////NL_SET_ERR_MSG(extack, "Handler is already registered");
			ret = -EEXIST;
		}
	}

	if (ret)
		return ret;

	hnd = kmalloc(sizeof(*hnd), GFP_KERNEL);
	if (unlikely(!hnd)) {
		////NL_SET_ERR_MSG(extack, "No mem for subhandler");
		goto hnd_nomem;
	}

	hnd->hnd_arg = hnd_arg;
	hnd->hnd_recv = recv;
	list_add_rcu(&hnd->list, &st->handlers);

	if (st_is_new) {
		list_add_rcu(&st->list, &net->mpls.substacks);
	}


	return 0;

hnd_nomem:
	if (st_is_new)
		kfree(st);
	return -ENOMEM;
}
EXPORT_SYMBOL(mpls_substack_add);

int mpls_substack_del(struct net *net, u32 input_label,
		      void *hnd_arg/*,
		      struct netlink_ext_ack *extack*/)
{
	struct mpls_substack *st;
	struct mpls_subhandler *hnd;

	ASSERT_RTNL();

	rcu_read_lock();
	st = find_label_rcu(net, input_label);
	if (unlikely(!st)) {
		////NL_SET_ERR_MSG(extack, "Substack not found");
		//BUG()?
		rcu_read_unlock();
		return -ENOENT;
	}
	hnd = find_handler_rcu(st, hnd_arg);
	if (unlikely(!hnd)) {
		////NL_SET_ERR_MSG(extack, "Handler not found");
		//BUG()?
		rcu_read_unlock();
		return -ENOENT;
	}
	rcu_read_unlock();

	list_del_rcu(&hnd->list);
	synchronize_rcu();

	if (!list_empty(&st->handlers))
		goto exit;

	/* list of subhandlers for this input label is empty.
	 * Drop the substack entry.
	 */

	list_del_rcu(&st->list);
	synchronize_rcu();
	kfree(st);

exit:
	kfree(hnd);
	return 0;
}
EXPORT_SYMBOL(mpls_substack_del);

/* nlmon.c */
#include <asm/uaccess.h>
#include <asm/delay.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#include <net/sock.h>
#include <net/nlmon/netlinkmon.h>
#include <net/nlmon/nlmon.h>

#if defined(CONFIG_DRU_RT_3052_ESW)
atomic_t nlmon_is_active = ATOMIC_INIT(0);
#endif

#define MAX_PAYLOAD_LEN 64

static int nlmon_major = 0;

static struct sock * g_monitor_nl_sk;
static int g_monitor_nl_pid = 0;
static void kerSysInitNlMonSocket(void);
static void kerSysCleanupNlMonSocket(void);

//#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
static long nlmon_ioctl(struct file * flip, unsigned int command, unsigned long arg);
//#else
//static int nlmon_ioctl(struct inode * inode, struct file * flip, unsigned int command, unsigned long arg);
//#endif

static struct file_operations nlmon_fops =
{
	.unlocked_ioctl = nlmon_ioctl,
};

/*
static void kerSysRecvFrmNlMonTask(struct sock * sk, int len) {
	struct sk_buff * skb = NULL;
	skb = skb_dequeue(&sk->sk_receive_queue);
	if(skb != NULL) {
		printk(KERN_WARNING "unexpected skb received at %s \n", __FUNCTION__);
		kfree_skb(skb);
	}
	return;
}	
*/
static void kerSysRecvFrmNlMonTask(struct sk_buff * skb) {
	if(skb != NULL) {
		//farisey: ???????? why "unexected" ?
		printk(KERN_WARNING "unexpected skb received at %s \n", __FUNCTION__);
		kfree_skb(skb);
	}
}

void kerSysInitNlMonSocket(void) {
	struct netlink_kernel_cfg cfg = {
		.groups		= 0,
		.input		= kerSysRecvFrmNlMonTask,
		.cb_mutex	= NULL,
	};

	g_monitor_nl_sk = netlink_kernel_create(&init_net, NETLINK_MONITOR, &cfg);

	if(!g_monitor_nl_sk) {
		printk(KERN_ERR "failed to create a netlink socket for monitor\n");
		return;
	}
#if defined(CONFIG_DRU_RT_3052_ESW)
	atomic_set(&nlmon_is_active, 1);
#endif
}

void kerSysCleanupNlMonSocket(void) {
#if defined(CONFIG_DRU_RT_3052_ESW)
	atomic_set(&nlmon_is_active, 0);
#endif
	g_monitor_nl_pid = 0;
	sock_release(g_monitor_nl_sk->sk_socket);
}


static void deliver_tasklet_func(unsigned long skbptr)
{
	netlink_unicast(g_monitor_nl_sk, (struct sk_buff*)skbptr, g_monitor_nl_pid, MSG_DONTWAIT);
}

static DECLARE_TASKLET(d_nl_tasklet, deliver_tasklet_func, 0);

static inline int deliver_nl_unicast(struct sock *ssk, struct sk_buff *skb, int pid, int flags)
{
	int curr_state;

	if (!in_irq() && !in_atomic())
		return netlink_unicast(ssk, skb, pid, flags);
	if (unlikely(!in_irq())) {
		/*
		 * farisey: Thu Nov 13 16:20:23 MSK 2014
		 *
		 * проверка "на всякий случай".
		 * я не могу себе представить вариант, когда
		 * это будет вызвано в атомарном контексте, но не
		 * из аппаратного прерывания.
		 * Замечание: в ra/mtk ядре это вызывается из
		 * esw_interrupt() и этого точно не будет.
		 * PS. Я хз что делать, если это сработает...
		 * Пока не делаю ничего особого, просто ворнинг выдаю.
		 */
		printk(KERN_WARNING "paranoid raised in %s:%d !\n", __FUNCTION__, __LINE__);
	}
	/* предполагаем контекст аппаратного прерывания */
	curr_state = test_and_set_bit(TASKLET_STATE_SCHED, &d_nl_tasklet.state);
	if (likely(!curr_state)) {
		//state == 0 => ready to schedule
		d_nl_tasklet.data = (unsigned long)skb;
		__tasklet_schedule(&d_nl_tasklet);
	}
	else {
		printk(KERN_WARNING "scheduling while not ready (%d) in %s:%d !\n",
		       curr_state, __FUNCTION__, __LINE__);
		dev_kfree_skb_any( (struct sk_buff*)d_nl_tasklet.data );//free old
		d_nl_tasklet.data = (unsigned long)skb;//replace with new
	}
	return 0;
}

void kerSysSendtoNlMonTask(int msgType, char * msgData, int msgDataLen){
	struct sk_buff * skb =  NULL;
	struct nlmsghdr * nl_msgHdr = NULL;
	unsigned int payloadLen = sizeof(struct nlmsghdr);

#if defined(CONFIG_NETLINK_MONITOR_LAN_LED)
	if (msgType == MSG_NETLINK_LINK_STATUS_CHANGED) {
		struct nlmon_msg_data *data = (struct nlmon_msg_data *)msgData;
		dru_led_link_changed(data->phy, data->status);
	}
#endif

	if(!g_monitor_nl_pid) {
		printk (KERN_INFO "message received before monitor task is initialized %s \n", __FUNCTION__);
		return;
	} 
	
	if(msgData && (msgDataLen > MAX_PAYLOAD_LEN)) {
		printk (KERN_ERR "invalid message len in %s", __FUNCTION__);
		return;
	} 

	payloadLen += msgDataLen;
	payloadLen = NLMSG_SPACE(payloadLen);
	
	if(in_atomic()) {
		skb = alloc_skb(payloadLen, GFP_ATOMIC);
	} else {
		skb = alloc_skb(payloadLen, GFP_KERNEL);
	}

	if(!skb) {
		printk (KERN_ERR "failed to alloc skb in %s", __FUNCTION__);
		return;
	}

      
	nl_msgHdr = (struct nlmsghdr*) skb->data;
	nl_msgHdr->nlmsg_type = msgType;
	nl_msgHdr->nlmsg_pid = 0; /* from kernel */
	nl_msgHdr->nlmsg_len = payloadLen;
	nl_msgHdr->nlmsg_flags = 0;

	
	if(msgData) {
		memcpy(NLMSG_DATA(nl_msgHdr), msgData, msgDataLen);
	}
	//absent in 3.10.14 NETLINK_CB(skb).pid = 0; /* from kernel */
	skb->len = payloadLen; 
	deliver_nl_unicast(g_monitor_nl_sk, skb, g_monitor_nl_pid, MSG_DONTWAIT);
	return;
}

//#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
static long nlmon_ioctl(struct file * flip, unsigned int command, unsigned long arg) {
//#else
//static int nlmon_ioctl(struct inode * inode, struct file * flip, unsigned int command, unsigned long arg) {
//#endif
	int ret = 0;
	NLMON_IOCTL_PARMS ctrlParms;
	
	switch(command) {
	case NLMON_IOCTL_SET_MONITOR_FD:
		if (copy_from_user ((void*) &ctrlParms, (void*) arg, sizeof(ctrlParms)) == 0) {		
			g_monitor_nl_pid =  ctrlParms.offset;
			printk(KERN_INFO "monitor task is initialized pid= %d \n", g_monitor_nl_pid);
		}
		break;
				
	case NLMON_IOCTL_WAKEUP_MONITOR_TASK:
		kerSysSendtoNlMonTask(MSG_NETLINK_WAKEUP_MONITOR_TASK, NULL, 0);
		break;
		
	default:
		ret = -EINVAL;
		ctrlParms.result = 0;
		printk(KERN_ERR "nlmon_ioctl: invalid command %x, cmd %d .\n", command, _IOC_NR(command));
		break;
						
	}
	
    return ret;	
}

static int __init nlmon_init(void) {
	int ret;

#if defined(CONFIG_NETLINK_MONITOR_LED_CONTROL)
	dru_led_init();
#endif
	ret = register_chrdev(NLMON_DRV_MAJOR, NLMON_DRV_NAME, &nlmon_fops);
	if (ret < 0) {
		printk(KERN_ERR "nlmon_init (major %d): fail to register device.\n", NLMON_DRV_MAJOR);
	} else {
		printk(KERN_INFO "nlmon: nlmon_init entry\n");
		nlmon_major = NLMON_DRV_MAJOR;
		kerSysInitNlMonSocket();
	}
	return ret;
}

void __exit nlmon_cleanup(void) {
	printk(KERN_INFO "nlmon_cleanup()\n");
	if (nlmon_major != -1) {
		kerSysCleanupNlMonSocket();
		unregister_chrdev(nlmon_major, NLMON_DRV_NAME);
	}
#if defined(CONFIG_NETLINK_MONITOR_LED_CONTROL)
	dru_led_fini();
#endif
}

#if defined(CONFIG_NETLINK_MONITOR_LED_CONTROL)
fs_initcall(nlmon_init); //вперёд встроенного raether или других
#else
module_init(nlmon_init);
#endif
module_exit(nlmon_cleanup);
EXPORT_SYMBOL(kerSysSendtoNlMonTask);

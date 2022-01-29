
#include "mtk_baseDefs.h"
#include "mtk_hwAccess.h"
#include "mtk_AdapterInternal.h"
#include "mtk_hwDmaAccess.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/mach-ralink/surfboardint.h>
#include "mtk_pecApi.h"
#include <net/mtk_esp.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry *entry;

extern void 
mtk_ipsec_init(
	void
);

extern void
mtk_ipsec_uninit(
	void
);

void
VDriver_Exit(
	void
);
static bool Adapter_IsInitialized = false;


static bool
Adapter_Init(
	void
)
{
    if (Adapter_IsInitialized != false)
    {
        printk("Adapter_Init: Already initialized\n");
        return true;
    }


    if (!HWPAL_DMAResource_Init(1024))
    {
		printk("HWPAL_DMAResource_Init failed\n");
       return false;
    }

    if (!Adapter_EIP93_Init())
    {
        printk("Adapter_EIP93_Init failed\n");
		return false;
    }

#ifdef ADAPTER_EIP93PE_INTERRUPTS_ENABLE
    Adapter_Interrupts_Init(SURFBOARDINT_CRYPTO);
#endif

    Adapter_IsInitialized = true;

    return true;
}


static void
Adapter_UnInit(
	void
)
{
    if (!Adapter_IsInitialized)
    {
        printk("Adapter_UnInit: Adapter is not initialized\n");
        return;
    }

    Adapter_IsInitialized = false;



    Adapter_EIP93_UnInit();

#ifdef ADAPTER_EIP93PE_INTERRUPTS_ENABLE
    Adapter_Interrupts_UnInit();
#endif

    HWPAL_DMAResource_UnInit();
}
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,36)
static int mcrypto_proc_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len, i;
    if (off > 0)
    {
        return 0;
    }

    len = sprintf(buf, "expand : %d\n", mcrypto_proc.copy_expand_count);
    len += sprintf(buf + len, "nolinear packet : %d\n", mcrypto_proc.nolinear_count);
    len += sprintf(buf + len, "oom putpacket : %d\n", mcrypto_proc.oom_in_put);
    for (i = 0; i < 16; i++)
    	len += sprintf(buf + len, "skbq[%d] : %d\n", i, mcrypto_proc.qlen[i]);
    for (i = 0; i < 16; i++)
    	len += sprintf(buf + len, "dbgpt[%d] : %d\n", i, mcrypto_proc.dbg_pt[i]);	
    return len;
}
#else
#include <linux/seq_file.h>
static int mcrypto_proc_read(struct seq_file *seq, void *v)
{
    int len, i;
	ipsecEip93Adapter_t *currAdapterPtr;
		
	mcrypto_proc.chstatus = 0;
		
	for (i = 0; i < IPESC_EIP93_ADAPTERS; i++)
	{
		mcrypto_proc.qlen[i] = 0;
		mcrypto_proc.qlen[i+IPESC_EIP93_ADAPTERS] = 0;
		currAdapterPtr = ipsecEip93AdapterListOut[i];
		if (currAdapterPtr)
		{	
			if (currAdapterPtr->status==TBL_DEL)
				mcrypto_proc.chstatus|=(1<<i);
			mcrypto_proc.qlen[i] = 0x0000FFFF &(currAdapterPtr->packet_count);
			mcrypto_proc.qlen[i+IPESC_EIP93_ADAPTERS] = 0x0000FFFF & (currAdapterPtr->skbQueue.qlen);
		}
	}
	for (i = 0; i < IPESC_EIP93_ADAPTERS; i++)
	{
		currAdapterPtr = ipsecEip93AdapterListIn[i];
		if (currAdapterPtr)
		{
			if (currAdapterPtr->status==TBL_DEL)
				mcrypto_proc.chstatus|=(1<<(i+IPESC_EIP93_ADAPTERS));	
			mcrypto_proc.qlen[i] |= currAdapterPtr->packet_count<<16;
			mcrypto_proc.qlen[i+IPESC_EIP93_ADAPTERS] |= currAdapterPtr->skbQueue.qlen<<16;
		}
	}
	for (i = 0; i < 10; i++)
		seq_printf(seq, "ipicpu[%d] : %d\n", i,mcrypto_proc.ipicpu[i]);
    seq_printf(seq, "expand : %d\n", mcrypto_proc.copy_expand_count);
    seq_printf(seq, "nolinear packet : %d\n", mcrypto_proc.nolinear_count);
    seq_printf(seq, "oom putpacket : %d\n", mcrypto_proc.oom_in_put);
	seq_printf(seq, "ch status : %08X\n", mcrypto_proc.chstatus);

	for (i = 0; i < 16; i++)
		seq_printf(seq, "ch[%d] Out %d : In %d\n", i, (short)(mcrypto_proc.qlen[i]&0x0000FFFF),\
			   	(short)(mcrypto_proc.qlen[i]>>16));
	for (i = 16; i < 32; i++)
        seq_printf(seq, "ch[%d] qOut %d : qIn %d\n", i-16, mcrypto_proc.qlen[i]&0x0000FFFF, mcrypto_proc.qlen[i]>>16);
                	 
    for (i = 0; i < 16; i++)
    	seq_printf(seq, "dbgpt[%d] : %08X\n", i, mcrypto_proc.dbg_pt[i]);	
    return 0;
}

static int mcrypto_open(struct inode *inode, struct file *file)
{
    return single_open(file, mcrypto_proc_read, NULL);
}

static const struct file_operations mcrypto_fops = {
    .owner      = THIS_MODULE,
    .open       = mcrypto_open,
    .read       = seq_read,
    .write			= NULL,
    .llseek     = seq_lseek,
    .release    = single_release
};
#endif

int
VDriver_Init(
	void
)
{
    
    if (!Adapter_Init())
    {
		printk("\n !Adapter_Init failed! \n");
        return -1;
    }

	if (PEC_Init(NULL) == PEC_ERROR_BAD_USE_ORDER)
	{
		printk("\n !PEC is initialized already! \n");
		return -1;
	}
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,36)
	entry = create_proc_entry(PROCNAME, 0666, NULL);
	if (entry == NULL)
	{
		printk("HW Crypto : unable to create /proc entry\n");
		return -1;
	}
	entry->read_proc = mcrypto_proc_read;
	entry->write_proc = NULL;
#else
	if (!(entry = proc_create(PROCNAME, 0, NULL, &mcrypto_fops)))
	{
		printk("HW Crypto : unable to create /proc entry\n");
		VDriver_Exit();
		return -1;
	}
#endif	
	memset(&mcrypto_proc, 0, sizeof(mcrypto_proc_type));
    
	mtk_ipsec_init();
    
    return 0;   // success
}



void
VDriver_Exit(
	void
)
{
	mtk_ipsec_uninit();
    Adapter_UnInit();
	
	PEC_UnInit();
	remove_proc_entry(PROCNAME, NULL);
}

MODULE_LICENSE("GPL");

module_init(VDriver_Init);
module_exit(VDriver_Exit);

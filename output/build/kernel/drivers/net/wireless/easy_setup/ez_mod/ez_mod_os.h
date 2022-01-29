#ifndef __EZ_MOD_OS_H__
#define __EZ_MOD_OS_H__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>



typedef spinlock_t			OS_NDIS_SPIN_LOCK;

/* */
/*  spin_lock enhanced for Nested spin lock */
/* */
#define OS_NdisAllocateSpinLock(__lock)			\
{                                       		\
    spin_lock_init((spinlock_t *)(__lock));		\
}

#define OS_NdisFreeSpinLock(lock)				\
	do{}while(0)


#define OS_SEM_LOCK(__lock)						\
{												\
	spin_lock_bh((spinlock_t *)(__lock));		\
}

#define OS_SEM_UNLOCK(__lock)					\
{												\
	spin_unlock_bh((spinlock_t *)(__lock));		\
}

#endif

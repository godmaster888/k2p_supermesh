#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#define RECONFIG_PARTITION_SIZE 1

#define MTD_BOOT_PART_SIZE  0x80000
#define MTD_CONFIG_PART_SIZE    0x20000
#define MTD_FACTORY_PART_SIZE   0x20000

extern unsigned int  CFG_BLOCKSIZE;
#define LARGE_MTD_BOOT_PART_SIZE       (CFG_BLOCKSIZE<<2)
#define LARGE_MTD_CONFIG_PART_SIZE     (CFG_BLOCKSIZE<<2)
#define LARGE_MTD_FACTORY_PART_SIZE    (CFG_BLOCKSIZE<<1)

//dlink
/* IMAGE1_SIZE_NEW is used in mtk_nand.c to modify partitions table */
extern unsigned int MTD_SIZE;

#if defined(CONFIG_DUAL_IMAGE) && !defined(CONFIG_USE_CUSTOM_IMAGE1_SIZE)
#define IMAGE1_SIZE_NEW			(MTD_SIZE / 2)
#else
/* if it's not dualboot, or if it's dualboot, but image size is set by config option - don't change image size*/
#define IMAGE1_SIZE_NEW			IMAGE1_SIZE
#endif
//end dlink

#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
#define MTD_ROOTFS_RESERVED_BLOCK	0x80000  // (CFG_BLOCKSIZE<<2)
#endif

#include "../maps/ralink-flash.h"

/*=======================================================================*/
/* NAND PARTITION Mapping                                                  */
/*=======================================================================*/
#if defined(CONFIG_SUPPORT_OPENWRT)
static struct mtd_partition g_pasStatic_Partition[] = {
	{
	        name:           "ALL",
	        size:           MTDPART_SIZ_FULL,
	        offset:         0,
	},
	/* Put your own partition definitions here */
	{
	        name:           "Bootloader",
	        size:           MTD_BOOT_PART_SIZE,
	        offset:         0,
	}, {
	        name:           "Config",
	        size:           MTD_CONFIG_PART_SIZE,
	        offset:         MTDPART_OFS_APPEND
	}, {
	        name:           "Factory",
	        size:           MTD_FACTORY_PART_SIZE,
	        offset:         MTDPART_OFS_APPEND
	}, {
	        name:           "firmware",
	        size:           MTDPART_SIZ_FULL,
	        offset:         0x140000,
	},
};
#else /* CONFIG_SUPPORT_OPENWRT */
//#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition g_pasStatic_Partition[] = {
	{
                name:           "ALL",
                size:           MTDPART_SIZ_FULL,
                offset:         0,
        },
        /* Put your own partition definitions here */
        {
                name:           "Bootloader",
                size:           MTD_BOOT_PART_SIZE,
                offset:         0,
        }, {
                name:           "Config",
                size:           MTD_CONFIG_PART_SIZE,
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Factory",
                size:           MTD_FACTORY_PART_SIZE,
                offset:         MTDPART_OFS_APPEND
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
        }, {
                name:           "Kernel",
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
        }, {
                name:           "RootFS",
                size:           MTD_ROOTFS_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
/*        }, {
                name:           "RootFS_reserved",
                size:           MTD_ROOTFS_RESERVED_BLOCK,
                offset:         MTDPART_OFS_APPEND,
*/
// dlink
        }, {
                name:           "Linux", // общий раздел Kernel + RootFS
                size:           MTDPART_SIZ_FULL - MTD_BOOT_PART_SIZE - MTD_CONFIG_PART_SIZE - MTD_FACTORY_PART_SIZE,
                offset:                            MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE + MTD_FACTORY_PART_SIZE,
//end dlink
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
        }, {
                name:           "Kernel_RootFS",
                size:           MTD_KERN_PART_SIZE + MTD_ROOTFS_PART_SIZE,
                offset:         MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE + MTD_FACTORY_PART_SIZE,
#endif
#else //CONFIG_RT2880_ROOTFS_IN_RAM
        }, {
                name:           "Kernel",
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#endif
#ifdef CONFIG_DUAL_IMAGE
// dlink
		}, {
				name:           "SafeMode",
				size:           SAFEMODE_SIZE,
				offset:         SAFEMODE_ADDR,
		}, {
				name:           "Config2",
				size:           MTD_CONFIG_PART_SIZE,
				offset:         MTD_CONFIG2_PART_OFFSET,
// end dlink
        }, {
                name:           "Kernel2",
                size:           MTD_KERN2_PART_SIZE,
                offset:         MTD_KERN2_PART_OFFSET,
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
        }, {
                name:           "RootFS2",
                size:           MTD_ROOTFS2_PART_SIZE,
                offset:         MTD_ROOTFS2_PART_OFFSET,
// dlink
		}, {
				name:           "Linux2",
				size:           IMAGE1_SIZE - (SAFEMODE_SIZE + MTD_CONFIG_PART_SIZE),
				offset:         MTD_KERN2_PART_OFFSET,
// end dlink
#endif
#endif
// dlink
#ifdef CONFIG_PART_FOR_BB_SKIP
		}, {
				name:           "Image", // Linux + Linux2 (если есть) + область для корректного скипа бедблоков
				size:           IMAGE1_SIZE,
				offset:         0,
#endif // CONFIG_PART_FOR_BB_SKIP
// end dlink
        }

};

#endif /* CONFIG_SUPPORT_OPENWRT */
#define NUM_PARTITIONS ARRAY_SIZE(g_pasStatic_Partition)
extern int part_num;	// = NUM_PARTITIONS;
//#endif
#undef RECONFIG_PARTITION_SIZE


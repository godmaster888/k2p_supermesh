/*****************************************************************************/

/*
 *	inode.c  --  Inode/Dentry functions for the USB device file system.
 *
 *	Copyright (C) 2000 Thomas Sailer (sailer@ife.ee.ethz.ch)
 *	Copyright (C) 2001,2002,2004 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  History:
 *   0.1  04.01.2000  Created
 *   0.2  10.12.2001  converted to use the vfs layer better
 */

/*****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/usb.h>
#include <linux/namei.h>
#include <linux/usbdevice_fs.h>
#include <linux/parser.h>
#include <linux/notifier.h>
#include <linux/seq_file.h>
// #include <linux/smp_lock.h>
#include <linux/usb/hcd.h>
#include <asm/byteorder.h>
#include "usb.h"

static int proc_initialized;	/* = 0 */
static struct proc_dir_entry *usbdir = NULL;

static void usbfs_add_bus(struct usb_bus *bus)
{
	struct dentry *parent;
	char name[8];
	int retval;

	if (!proc_initialized)
		return -EACCES;

	sprintf (name, "%03d", bus->busnum);

	bus->usbfs_dentry = proc_mkdir(name, usbdir);
	if (!bus->usbfs_dentry) {
		printk(KERN_ERR "Error creating usbfs bus entry\n");
		return;
	}
}

static void usbfs_remove_bus(struct usb_bus *bus)
{
	proc_remove(bus->usbfs_dentry);
}

static void usbfs_add_device(struct usb_device *dev)
{
	struct proc_dir_entry *ent;
	char name[8];

	if (!proc_initialized)
		return -EACCES;

	sprintf (name, "%03d", dev->devnum);

	ent = proc_create_data(name, S_IFREG | S_IRUGO | S_IWUSR, dev->bus->usbfs_dentry, &usbdev_file_operations, dev);
	if (!ent) {
		printk(KERN_ERR "Error creating usbfs device entry\n");
		return;
	}

	dev->usbfs_dentry = ent;
}

static void usbfs_remove_device(struct usb_device *dev)
{
	if (dev->usbfs_dentry) {
		proc_remove(dev->usbfs_dentry);
		dev->usbfs_dentry = NULL;
	}
}

static int usbfs_notify(struct notifier_block *self, unsigned long action, void *dev)
{
	switch (action) {
	case USB_DEVICE_ADD:
		usbfs_add_device(dev);
		break;
	case USB_DEVICE_REMOVE:
		usbfs_remove_device(dev);
		break;
	case USB_BUS_ADD:
		usbfs_add_bus(dev);
		break;
	case USB_BUS_REMOVE:
		usbfs_remove_bus(dev);
	}

	usbfs_conn_disc_event();
	return NOTIFY_OK;
}

static struct notifier_block usbfs_nb = {
	.notifier_call = 	usbfs_notify,
};

int __init usbfs_init(void)
{
	usb_register_notify(&usbfs_nb);

	/* create mount point for usbfs */
	usbdir = proc_mkdir("bus/usb", NULL);

	proc_create("devices", 0, usbdir, &usbfs_devices_fops);
	proc_initialized = 1;

	return 0;
}

void usbfs_cleanup(void)
{
	usb_unregister_notify(&usbfs_nb);
	if (usbdir)
		remove_proc_entry("bus/usb", NULL);
}


/*
 *  linux/fs/ext3/file.c
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/file.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  ext3 fs regular file handling primitives
 *
 *  64-bit file support on 64-bit platforms by Jakub Jelinek
 *	(jj@sunsite.ms.mff.cuni.cz)
 */

#include <linux/time.h>
#include <linux/quotaops.h>
#include "ext3.h"
#include "xattr.h"
#include "acl.h"
#include <linux/gps.h>

// typedef  __u32 float;
// typedef  __u64 double;

extern struct gps_location *local_kernel;

/*
 * Called when an inode is released. Note that this is different
 * from ext3_file_open: open gets called at every open, but release
 * gets called only when /all/ the files are closed.
 */
static int ext3_release_file (struct inode * inode, struct file * filp)
{
	if (ext3_test_inode_state(inode, EXT3_STATE_FLUSH_ON_CLOSE)) {
		filemap_flush(inode->i_mapping);
		ext3_clear_inode_state(inode, EXT3_STATE_FLUSH_ON_CLOSE);
	}
	/* if we are the last writer on the inode, drop the block reservation */
	if ((filp->f_mode & FMODE_WRITE) &&
			(atomic_read(&inode->i_writecount) == 1))
	{
		mutex_lock(&EXT3_I(inode)->truncate_mutex);
		ext3_discard_reservation(inode);
		mutex_unlock(&EXT3_I(inode)->truncate_mutex);
	}
	if (is_dx(inode) && filp->private_data)
		ext3_htree_free_dir_info(filp->private_data);

	return 0;
}

static int ext3_file_set_gps_location(struct inode *file_inode)
{
	struct ext3_inode_info *ei = EXT3_I(file_inode);
	ei->i_latitude = *(unsigned long long *)&local_kernel->latitude;
	ei->i_longitude = *(unsigned long long *)&local_kernel->longitude;
	ei->i_accuracy = *(unsigned int *)&local_kernel->accuracy;
	
	/*update i_coord_age*/
	struct timeval ltime;
   	do_gettimeofday(&ltime);
   	ei->i_coord_age = (u32)(ltime.tv_sec - (sys_tz.tz_minuteswest * 60))-ei->i_coord_age;

	return 0;
}

static int ext3_file_get_gps_location(struct inode *file_inode, struct gps_location *loc)
{
	struct ext3_inode_info *ei = EXT3_I(file_inode);
	*(unsigned long long *)&loc->latitude = ei->i_latitude;
	*(unsigned long long *)&loc->longitude = ei->i_longitude;
	*(unsigned int *)&loc->accuracy = ei->i_accuracy;

	return 0;
}

const struct file_operations ext3_file_operations = {
	.llseek		= generic_file_llseek,
	.read		= do_sync_read,
	.write		= do_sync_write,
	.aio_read	= generic_file_aio_read,
	.aio_write	= generic_file_aio_write,
	.unlocked_ioctl	= ext3_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= ext3_compat_ioctl,
#endif
	.mmap		= generic_file_mmap,
	.open		= dquot_file_open,
	.release	= ext3_release_file,
	.fsync		= ext3_sync_file,
	.splice_read	= generic_file_splice_read,
	.splice_write	= generic_file_splice_write,
};

const struct inode_operations ext3_file_inode_operations = {
	.setattr	= ext3_setattr,
#ifdef CONFIG_EXT3_FS_XATTR
	.setxattr	= generic_setxattr,
	.getxattr	= generic_getxattr,
	.listxattr	= ext3_listxattr,
	.removexattr	= generic_removexattr,
#endif
	.get_acl	= ext3_get_acl,
	.fiemap		= ext3_fiemap,
	.set_gps_location = ext3_file_set_gps_location,
	.get_gps_location = ext3_file_get_gps_location,
};


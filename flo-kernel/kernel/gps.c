#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/uaccess.h>
#include <linux/limits.h>
#include <linux/err.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

struct gps_location *local_kernel;

static void __init init_local_kernel(void)
{
	local_kernel = (struct gps_location *)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
}

/**
 * Judge if the gps_location is all zero.
 * return 0 if valid, return -1 on not valid.
**/
static int isGpsValid(struct gps_location *loc) {
	if (loc == NULL)
		return -1;
	double double0 = 0;
	float float0 = 0;
	u64 dzero = *((unsigned long long *)(&double0));
	u32 fzero = *((unsigned long *)(&float0));
	u64 lat, lng;
	u32 acc;

	lat = *((unsigned long long *)(&loc->latitude));
	lng = *((unsigned long long *)(&loc->longitude));
	acc = *((unsigned long *)(&loc->accuracy));
	if (lat == dzero && lng == dzero && acc == fzero)
		return -1;
	return 0;
}

SYSCALL_DEFINE(set_gps_location) (struct gps_location *loc) {
	if (isGpsValid(loc) == -1)
		return -EINVAL;
	int ret = copy_from_user(local_kernel, loc, sizeof(struct gps_location));
	if (ret != 0)
		return -EFAULT;
	return 0;
}



SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname,
	struct gps_location __user *, loc) {
	int getret;

	if (pathname == NULL || loc == NULL){
		printk("pathname == NULL || loc == NULL");
		return -EINVAL;
	}

	struct gps_location loc_k;
	char *pathname_k;

	pathname_k = kmalloc((PATH_MAX+1)*sizeof(char), GFP_KERNEL);
	if (pathname_k == NULL) {
		printk("pathname_k == NULL");
		kfree(pathname_k);
		return -ENOMEM;
	}
	int cpy_ret;

	cpy_ret = strncpy_from_user(pathname_k, pathname, PATH_MAX+1);
	if (cpy_ret < 0) {
		printk("cpy_ret < 0");
		kfree(pathname_k);
		return -EFAULT;
	} else if (cpy_ret >= PATH_MAX + 1) {
		printk("cpy_ret >= PATH_MAX + 1");
		kfree(pathname_k);
		return -EINVAL;
	}

	int access_ret;
	access_ret = sys_faccessat(AT_FDCWD, pathname_k, 4);
	if (access_ret < 0) {
		printk("cannot access file!\n");
		kfree(pathname_k);
		return -EINVAL;
	}
	// struct file * kfile;
	// kfile = filp_open(pathname_k, O_RDWR, 0);
	// if (IS_ERR(kfile)){
	// 	printk("IS_ERR(kfile)");
	// 	return -EINVAL;
	// }
	// struct inode *filenode;
	// filenode = kfile->f_path.dentry->d_inode;

	// int getret;
	// if (filenode == NULL){
	// 	printk("filenode == NULL");
	// 	return -EINVAL;
	// }
	/* change a method, using link_path_walk instead of filp_open */
	struct inode *filenode;
	struct path path;
	getret = kern_path(pathname_k, LOOKUP_FOLLOW,&path);
	filenode = path.dentry->d_inode;
	if(getret != 0 || filenode == NULL) {
		printk("filenode == NULL");
		kfree(pathname_k);
		return -EINVAL;
	}
	if (filenode->i_op->get_gps_location != NULL)
		getret = filenode->i_op->get_gps_location(filenode, loc);
	else {
		printk("filenode->i_op->get_gps_location == NULL");
		kfree(pathname_k);
		return -EINVAL;
	}
	kfree(pathname_k);
	return getret;
}

module_init(init_local_kernel);

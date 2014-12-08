#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/uaccess.h>
#include <linux/limits.h>
#include <linux/err.h>

struct gps_location *local_kernel;

/**
 * Init the kernel gps variable at the very beginning
**/
static void __init init_local_kernel(void)
{
	local_kernel = kmalloc(sizeof(struct gps_location),
				GFP_KERNEL);
}
/**
 * Check if the gps_location is all zero.
 * return 0 if valid, return -1 on not valid.
**/
static int isGpsValid(struct gps_location *loc)
{
	if (loc == NULL)
		return -EINVAL;
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
		return -EINVAL;
	return 0;
}

SYSCALL_DEFINE(set_gps_location) (struct gps_location *loc) {
	if (isGpsValid(loc) != 0)
		return -EINVAL;
	int ret = copy_from_user(local_kernel,
		loc, sizeof(struct gps_location));
	if (ret != 0)
		return -EFAULT;
	return 0;
}

SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname,
	struct gps_location __user *, loc) {
	int getret;

	/*
	* check if the inputs are null
	*/
	if (pathname == NULL || loc == NULL)
		return -EINVAL;

	struct gps_location loc_k;
	char *pathname_k;

	pathname_k = kmalloc((PATH_MAX+1)*sizeof(char), GFP_KERNEL);
	/*
	* check if kmalloc is success
	*/
	if (pathname_k == NULL) {
		kfree(pathname_k);
		return -ENOMEM;
	}
	int cpy_ret;

	/*
	* copy the file path from user and check if success
	*/
	cpy_ret = strncpy_from_user(pathname_k, pathname, PATH_MAX+1);
	if (cpy_ret < 0) {
		kfree(pathname_k);
		return -EFAULT;
	} else if (cpy_ret >= PATH_MAX + 1) {
		kfree(pathname_k);
		return -EINVAL;
	}

	int access_ret;

	/* //check if the file is readable by the current user
	access_ret = sys_access((const char __user *) pathname_k, 4);
	if (access_ret < 0) {
		kfree(pathname_k);
		return -EINVAL;
	}
	*/

	/**
	* Get inode from the pathname, and check if success
	**/
	struct inode *filenode;
	struct path path;
	getret = kern_path(pathname_k, LOOKUP_FOLLOW, &path);
	filenode = path.dentry->d_inode;
	if (getret != 0 || filenode == NULL) {
		kfree(pathname_k);
		return -EINVAL;
	}

	struct gps_location * temploc;

	temploc = kmalloc(sizeof(struct gps_location), GFP_KERNEL);
	if (temploc == NULL) {
		kfree(pathname_k);
		return -ENOMEM;
	}

	if (filenode->i_op->get_gps_location != NULL)
		getret = filenode->i_op->get_gps_location(filenode, temploc);
	else {
		kfree(pathname_k);
		return -EINVAL;
	}

	if(isGpsValid(temploc) != 0){
		kfree(pathname_k);
		kfree(temploc);
		return -EINVAL;
	}

	int copyturet;

	copyturet = copy_to_user(loc, temploc, sizeof(struct gps_location));
	if (copyturet<0) {
		kfree(pathname_k);
		kfree(temploc);
		return -EFAULT;
	}
	kfree(pathname_k);
	kfree(temploc);
	return getret;
}

module_init(init_local_kernel);

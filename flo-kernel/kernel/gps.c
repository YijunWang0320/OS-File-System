#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/limits.h>

struct gps_location *local_kernel;

static void __init init_local_kernel(void)
{
	local_kernel = (struct gps_location *)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
}

SYSCALL_DEFINE(set_gps_location) (struct gps_location *loc) {
	struct gps_location *tmp_loc_kernel = (struct gps_location *)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
	int ret = copy_from_user(tmp_loc_kernel, loc, sizeof(struct gps_location));
	if(ret != 0)
		return -EFAULT;
	return 0;
}



SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname,
	struct gps_location __user *, loc) {
	if (pathname == NULL || loc == NULL)
		return -EINVAL;

	struct gps_location loc_k;
	char *pathname_k;

	pathname_k = kmalloc((PATH_MAX+1)*sizeof(char), GFP_KERNEL);

	return 0;
}

module_init(init_local_kernel);

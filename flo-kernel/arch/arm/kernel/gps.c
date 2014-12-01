#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
struct gps_location *local_kernel;

static void __init init_local_kernel(void)
{
	local_kernel = (struct gps_location *)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
}

SYSCALL_DEFINE(set_gps_location) (struct gps_location *loc) {
	struct gps_location *tmp_loc_kernel = (struct gps_location *)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
	int ret = copy_from_user(local_kernel, loc, sizeof(struct gps_location));
	if(ret != 0)
		return -1;

	printk("local_kernel->latitude: %d \n", local_kernel->latitude);

	printk("local_kernel->longitude: %d \n", local_kernel->longitude);


	return 10;
}

module_init(init_local_kernel);

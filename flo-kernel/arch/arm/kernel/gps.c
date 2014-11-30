#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
struct gps_location *local_kernel;
SYSCALL_DEFINE(set_gps_location) (struct gps_location *loc) {
	struct gps_location *tmp_loc_kernel = (struct gps_location *)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
	int ret = copy_from_user(&tmp_loc_kernel, loc, sizeof(struct gps_location));
	if(ret != 0)
		return -1;

	local_kernel->latitude = tmp_loc_kernel->latitude;
	local_kernel->longitude = tmp_loc_kernel->longitude;
	local_kernel->accuracy = tmp_loc_kernel->accuracy;


	return 10;
}

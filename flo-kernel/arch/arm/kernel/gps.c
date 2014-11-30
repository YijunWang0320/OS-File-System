#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/gps.h>

SYSCALL_DEFINE(set_gps_location) (struct gps_location *loc) {
	struct gps_location tmp_loc_kernel = malloc(sizeof(struct gps_location));
	int ret = copy_from_user(&tmp_loc_kernel, loc, sizeof(struct gps_location));
	if(ret != 0)
		return -1;

	local_kernel->latitude = tmp_loc_kernel->latitude;
	printk("pass latitude, loc_kernel is %f, tmp_loc_kernel is %f \n", local_kernel->latitude, tmp_loc_kernel->latitude);
	local_kernel->longitude = tmp_loc_kernel->longitude;
	printk("pass longitude, loc_kernel is %f, tmp_loc_kernel is %f \n", local_kernel->longitude, tmp_loc_kernel->longitude);
	local_kernel->accuracy = tmp_loc_kernel->accuracy;
	printk("pass accuracy, loc_kernel is %f, tmp_loc_kernel is %f \n", local_kernel->accuracy, tmp_loc_kernel->accuracy);


	return 10;
}
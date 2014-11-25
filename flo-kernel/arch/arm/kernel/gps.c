#include <linux/syscall.h>
#include <linux/unistd.h>
#include <linux/gps.h>

SYSCALL_DEFINE(set_gps_location, struct gps_location *, loc) {
	return 10;		
}
#include <linux/syscalls.h>
#include <linux/unistd.h>

SYSCALL_DEFINE(set_gps_location) (struct gps_location *loc) {
	return 10;	
}
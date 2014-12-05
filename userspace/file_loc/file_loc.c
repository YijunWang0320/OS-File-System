#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include "file_loc.h"

int main(int argc, char **argv)
{
	char *pathname;
	if (argc != 2) {
		printf("Wrong parameter!\n");
		return -1;
	}
	/* get the input argument as the path */
	pathname = argv[1];

	struct gps_location *loc = malloc(sizeof(struct gps_location));
	int ret = get_gps_location(pathname, loc);
	/* check if we call the syscall successfully */
	if (ret < 0) {
			free(loc);
			printf("Fail to get location information of the given path name!\n");
			return -1;
	}
	printf("GPS information:\n");
	printf("latitude : %f\n", loc->latitude);
	printf("longitude: %f\n", loc->longitude);
	printf("accuracy: %f\n", loc->accuracy);
	printf("Data age: %d s\n", ret);
	printf("Map URL:\n");
	printf("https://maps.googleapis.com/maps/api/staticmap?zoom=13&size=600x600&maptype=roadmap&markers=color:red|label:F|%f,%f",loc->latitude, loc->longitude);
	free(loc);
	return 0;
}

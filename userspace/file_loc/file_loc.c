#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_loc.h"

int main(int argc, char **argv)
{
	char *pathname = "/data/misc/hmwk6/test.txt";
	struct gps_location *loc = (struct gps_location *)malloc(sizeof(struct gps_location));
	int ret = get_gps_location(pathname, loc);
	printf("latitude : %f \n", loc->latitude);
	printf("longitude: %f \n", loc->longitude);
	printf("accuracy: %f \n", loc->accuracy);
	printf("timestamp: %d \n", ret);
	return 0;
}

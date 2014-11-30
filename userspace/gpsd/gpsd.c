#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gpsd.h"

int main(int argc, char *argv[])
{
	FILE *fp= NULL;
	gps_location * gpsl;
	pid_t pid = 0;
	pid_t sid = 0;
	pid = fork();
	if(pid < 0) {
		printf("fork failed. \n");
		exit(EXIT_FAILURE);
	}

	if(pid > 0) {
		exit(EXIT_SUCCESS);
	}
	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);
	if ((chdir("/"))< 0)
		exit(EXIT_FAILURE);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	gpsl = (struct gps_location *)malloc(sizeof(struct gps_location));
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	while(1) {
		fp = fopen("/data/media/0/gps_location.txt", "rt");
		read = getline(&line, &len, fp);
		gpsl->latitude = strtod(line, NULL);

		read = getline(&line, &len, fp);
		gpsl->longtitude = strtod(line, NULL);

		read = getline(&line, &len, fp);
		gpsl->accuracy = strtof(line, NULL);

j		int ret = set_gps_location(gpsl);
		fclose(fp);
		sleep(1);
	}
	return 0;
}


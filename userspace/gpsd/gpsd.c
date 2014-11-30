#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gpsd.h"

int main(int argc, char *argv[])
{
	FILE *fp= NULL;
	struct gps_location * gpsl;
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
		if (read != -1){
			gpsl->latitude = strtod(line, NULL);
			printf("latitude: %s\n", line);
		}
		else 
			break;

		if (read != -1) {
			gpsl->longitude = strtod(line, NULL);
			printf("latitude: %s\n", line);
		}
		else 
			break;

		if (read != -1) {
			gpsl->accuracy = strtof(line, NULL);
			printf("latitude: %s\n", line);
		}
		else 
			break;

		int ret = set_gps_location(gpsl);
		fclose(fp);
		sleep(1);
	}

	printf("while break because of error read. \n");

	return 0;
}


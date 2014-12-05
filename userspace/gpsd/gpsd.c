#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gpsd.h"

int main(int argc, char *argv[])
{
	/**
	* Check if the current user is root, or the daemon cannot
	* be executed.
	**/
	uid_t uid, euid;

	uid = getuid();
	euid = geteuid();
	if (uid < 0 || uid != euid) {
		printf("ERROR: only root user could run daemon!\n");
		exit(EXIT_FAILURE);
	}
	/**
	 * Fork a new process and turn it into a daemon.
	 * Exit the parent process to make sure just one
	 * daemon is running.
	**/
	FILE *fp = NULL;
	struct gps_location *gpsl;
	pid_t pid = 0;
	pid_t sid = 0;
	pid = fork();
	if (pid < 0) {
		printf("fork failed.\n");
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);
	if ((chdir("/")) < 0)
		exit(EXIT_FAILURE);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	gpsl = (struct gps_location *)malloc(sizeof(struct gps_location));
	if (gpsl == NULL)
		exit(EXIT_FAILURE);
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	/**
	 * Get the gps location from the text file writen by the user app.
	 * Write gps into the kernel useing set_gps_location syscall every
	 * one second.
	 *
	**/
	while (1) {
		fp = fopen("/data/media/0/gps_location.txt", "rt");
		if (fp == NULL) {
			free(gpsl);
			exit(EXIT_FAILURE);
		}
		read = getline(&line, &len, fp);
		if (read != -1)
			gpsl->latitude = strtod(line, NULL);
		else {
			free(gpsl);
			exit(EXIT_FAILURE);
		}
		read = getline(&line, &len, fp);
		if (read != -1)
			gpsl->longitude = strtod(line, NULL);
		else {
			free(gpsl);
			exit(EXIT_FAILURE);
		}
		read = getline(&line, &len, fp);
		if (read != -1)
			gpsl->accuracy = strtof(line, NULL);
		else {
			free(gpsl);
			exit(EXIT_FAILURE);
		}

		int ret = set_gps_location(gpsl);
		if (ret != 0) {
			free(gpsl);
			exit(EXIT_FAILURE);
		}
		fclose(fp);
		sleep(1);
	}

	return 0;
}


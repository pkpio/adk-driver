#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_DEVICE	"/dev/adb1"

void read_log(int fd)
{
	int retval = 0;
	char *log;
	
	retval = read(fd, log, 20);
	if (retval < 0)
		fprintf(stderr, "an error occured: %d\n", retval);
	else
		fprintf(stdout, "Read bytes: %d\n", retval);
}

int main (){	
	int fd;
	char *dev = DEFAULT_DEVICE;
	
	fd = open(dev, O_RDWR);
	if (fd == -1)
	{
		perror("open");
		exit(1);
	}
	
	fprintf(stdout, "The device is: %s\n", DEFAULT_DEVICE);

	read_log(fd);
	close(fd);
}

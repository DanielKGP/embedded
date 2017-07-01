#include <unistd.h>
#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>

int main() {
	int fd = open("/proc/proc",O_RDWR);
	char buf[1024] = "123456";
	int k;
	write(fd,buf,1024);
	bzero(buf,1024);
	scanf("%d",&k);
	read(fd,buf,1024);
	perror("error");
	printf("%s\n",buf);
	close(fd);
}

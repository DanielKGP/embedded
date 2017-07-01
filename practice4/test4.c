#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>

int main() {

  char *buffer = NULL;

	int fd = open("/dev/page",O_RDWR);

  buffer = (char*)mmap(0,524288,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

	char buf[16] = "123456";
	bzero(buf,16);
  int i = 0;
  for(i=0;i<1;i++)
  {
    memcpy(buf,buffer+(unsigned int)(i*4096),15);
    printf("%s",buf);
  }
	close(fd);
}

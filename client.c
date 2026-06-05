#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTNUM 8000 

int main(int argc, char **argv)
{
	struct sockaddr_in saddr;
	int                fd;
	char               *buf="Hello, Socket Programming\n";

	/* make server's socket */
	if((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family      = AF_INET;
	saddr.sin_port        = htons(PORTNUM);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	if (connect(fd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
		perror("connect");
		exit(-1);
	}

	send(fd, buf, strlen(buf), 0);

	close(fd);
	return 0;
}

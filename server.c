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
#include <pthread.h>

#define PORTNUM 8000 
#define BUFSIZE 4096

void *recv_and_resp(int *fd_socket)
{
	int  fd, ret;
	char buf[BUFSIZE];
	fd = (int)*fd_socket;
	
	if ((ret = recv(fd, buf, BUFSIZE, 0)) > 0) {
		write(1,buf,ret);
	}
	close(fd);
}

int main(void)
{
	struct sockaddr_in saddr, caddr;
	int                fd1, fd2, ret, len;
	pthread_t          pt;	

	/* make server's socket */
	if ((fd1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family      = AF_INET;
	saddr.sin_port        = htons(PORTNUM);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(fd1, (struct sockaddr*)&saddr, sizeof(saddr))) {
		perror("bind");
		return -1;
	}

	if(listen(fd1, 5)) {
		perror("listen");
		return -1;
	}

	for (;;) {
		len = sizeof(caddr);
		if((fd2 = accept(fd1, (struct sockaddr*)&caddr, &len)) < 0) {
			perror("accept");
			exit(1);
		}
		
		if (pthread_create(&pt, NULL, (void*)(recv_and_resp), (void*)&fd2) < 0) {
			perror("pthread_create");
			return -1;
		}
		pthread_detach(pt);
	}
	
	return 0;
}

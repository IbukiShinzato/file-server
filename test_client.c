#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORTNUM 8000
#define BUFSIZE 4096

void* get_client(void* arg)
{
    struct sockaddr_in saddr;
    int fd;
    char buf[BUFSIZE];
    char response[BUFSIZE];
    int file_number = *(int*)arg;
    free(arg);

    /* make server's socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket");
        return NULL;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORTNUM);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect");
        close(fd);
        return NULL;
    }

    snprintf(buf, BUFSIZE, "GET<test%d.txt>\n", file_number);
    send(fd, buf, strlen(buf), 0);

    int n = snprintf(response, BUFSIZE, "[test%d.txt]\n", file_number);
    if (n < 0 || n >= BUFSIZE)
    {
        perror("snprintf");
        close(fd);
        return NULL;
    }

    int ret = recv(fd, response + n, BUFSIZE - n, 0);

    if (ret == 0)
    {
        close(fd);
        return NULL;
    }

    if (ret < 0)
    {
        perror("recv");
        close(fd);
        return NULL;
    }

    write(1, response, ret + n);

    close(fd);

    return NULL;
}

int main(void)
{
    int i;
    pthread_t threads[16];

    for (i = 1; i <= 16; i++)
    {
        int* arg = malloc(sizeof(int));
        if (arg == NULL)
        {
            perror("malloc");
            return -1;
        }
        *arg = i;

        if (pthread_create(&threads[i - 1], NULL, get_client, arg) != 0)
        {
            printf("i: %d\n", i);
            perror("pthread_create");
            free(arg);
            return -1;
        }
    }

    for (i = 0; i < 16; i++)
    {
        pthread_join(threads[i], 0);
    }

    return 0;
}

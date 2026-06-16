#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
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

void send_file(int fd_socket, const char* file_name)
{
    char path[BUFSIZE];
    char file_buf[BUFSIZE];
    char response[BUFSIZE];

    char* home = getenv("HOME");
    snprintf(path, BUFSIZE, "%s/%s", home, file_name);
    int fd_file = open(path, O_RDONLY);

    if (fd_file < 0)
    {
        write(fd_socket, "NOT FOUND\n", 10);
        return;
    }

    int ret = read(fd_file, file_buf, BUFSIZE - 1);

    if (ret < 0)
    {
        close(fd_file);
        write(fd_socket, "NOT FOUND\n", 10);
        return;
    }

    file_buf[ret] = '\0';

    int len = snprintf(response, BUFSIZE, "FILE(%d): %s\n", ret, file_buf);

    write(fd_socket, response, len);

    close(fd_file);
}

void create_file(int fd_socket, const char* file_name, const char* content, int n)
{
    char path[BUFSIZE];

    char* home = getenv("HOME");
    snprintf(path, BUFSIZE, "%s/%s", home, file_name);
    int fd_file = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd_file < 0)
    {
        write(fd_socket, "NOT PERMISSION\n", 15);
        return;
    }

    int ret = write(fd_file, content, n);

    if (ret < 0)
    {
        close(fd_file);
        write(fd_socket, "NOT PERMISSION\n", 15);
        return;
    }

    write(fd_socket, "CREATED\n", 8);

    close(fd_file);
}

void delete_file(int fd_socket, const char* file_name)
{
    char path[BUFSIZE];

    char* home = getenv("HOME");
    snprintf(path, BUFSIZE, "%s/%s", home, file_name);

    int ret = unlink(path);

    if (ret < 0)
    {
        write(fd_socket, "NOT FOUND\n", 10);
        return;
    }

    write(fd_socket, "DELETED\n", 8);
}

void ls(int fd_socket)
{
    char response[BUFSIZE];
    int pos = 0;

    DIR* mydir;
    struct dirent* myfile;

    char* home = getenv("HOME");
    mydir = opendir(home);

    if (mydir == NULL)
    {
        write(fd_socket, "NOT PERMISSION\n", 15);
        return;
    }

    memset(response, 0, BUFSIZE);
    while ((myfile = readdir(mydir)) != NULL)
    {
        if (pos >= BUFSIZE - 1)
        {
            break;
        }
        snprintf(response + pos, BUFSIZE - pos, "%s\n", myfile->d_name);
        while (pos < BUFSIZE && response[pos] != '\0')
        {
            pos += 1;
        }
    }

    write(fd_socket, response, pos);
    closedir(mydir);
}

void* recv_and_resp(void* arg)
{
    int ret;
    char buf[BUFSIZE];
    char file_name[BUFSIZE];
    char content[BUFSIZE];
    int fd = *(int*)arg;
    free(arg);

    for (;;)
    {
        memset(buf, 0, BUFSIZE);
        memset(file_name, 0, BUFSIZE);
        memset(content, 0, BUFSIZE);

        ret = recv(fd, buf, BUFSIZE, 0);

        if (ret == 0)
        {
            break;
        }

        if (ret < 0)
        {
            perror("recv");
            break;
        }

        if (ret == 3 && strncmp(buf, "LS\n", 3) == 0)
        {
            ls(fd);
            continue;
        }

        if (ret < 6)
        {
            write(fd, "INVALID REQUEST\n", 16);
            continue;
        }

        if (strncmp(buf, "GET<", 4) == 0 && buf[ret - 2] == '>' && buf[ret - 1] == '\n')
        {
            int file_name_len = (ret - 3) - 4 + 1;
            memcpy(file_name, buf + 4, file_name_len);
            send_file(fd, file_name);
        }
        else if (strncmp(buf, "PUT<", 4) == 0 && buf[ret - 2] == '>' && buf[ret - 1] == '\n')
        {
            int i = 0;
            while (i < ret && buf[i] != '>')
            {
                i++;
            }
            int file_name_len = i - 4;
            memcpy(file_name, buf + 4, file_name_len);
            int content_start = i + 2;
            if (buf[i + 1] != '<')
            {
                write(fd, "INVALID REQUEST\n", 16);
                continue;
            }
            int content_len = (ret - 2) - content_start;
            memcpy(content, buf + content_start, content_len);
            create_file(fd, file_name, content, content_len);
        }
        else if (strncmp(buf, "DEL<", 4) == 0 && buf[ret - 2] == '>' && buf[ret - 1] == '\n')
        {
            int file_name_len = (ret - 3) - 4 + 1;
            memcpy(file_name, buf + 4, file_name_len);
            delete_file(fd, file_name);
        }
        else
        {
            write(fd, "INVALID REQUEST\n", 16);
            continue;
        }
    }

    close(fd);

    return NULL;
}

int main(void)
{
    struct sockaddr_in saddr, caddr;
    int fd1, fd2;
    pthread_t pt;

    /* make server's socket */
    if ((fd1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket");
        return -1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORTNUM);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd1, (struct sockaddr*)&saddr, sizeof(saddr)))
    {
        perror("bind");
        return -1;
    }

    if (listen(fd1, 5))
    {
        perror("listen");
        return -1;
    }

    for (;;)
    {
        socklen_t len = sizeof(caddr);
        if ((fd2 = accept(fd1, (struct sockaddr*)&caddr, &len)) < 0)
        {
            perror("accept");
            exit(1);
        }

        int* arg_fd = malloc(sizeof(int));
        if (arg_fd == NULL)
        {
            perror("malloc");
            return -1;
        }
        *arg_fd = fd2;

        if (pthread_create(&pt, NULL, recv_and_resp, arg_fd) != 0)
        {
            perror("pthread_create");
            free(arg_fd);
            close(fd2);
            return -1;
        }
        pthread_detach(pt);
    }

    return 0;
}

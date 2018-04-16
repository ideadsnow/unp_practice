//
// Created by ids on 4/16/18.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <signal.h>
#include <wait.h>
#include "../unp.h"

void str_echo(int sockfd);

void sig_chld(int signo);

int main() {
    int listenfd, connfd;
    __socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, SOMAXCONN);

    signal(SIGCHLD, sig_chld); // Ubuntu 17.10 no need???

    while (1) {
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                printf("accept error. errno: %d", errno);
                exit(0);
            }
        }

        if (fork() == 0) // child_pid == 0, it's a child process
        {
            close(listenfd);
            str_echo(connfd);
            printf("child process exit.");
            exit(0);
        }

        close(connfd);
    }
}

void print_str(char *s, ssize_t len) {
    int i;
    for (i = 0; i < len; i++) {
        printf("%c", s[i]);
    }
}


void str_echo(int sockfd) {
    ssize_t n;
    char buf[MAXLINE];

    again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        print_str(buf, n);
        write(sockfd, buf, (size_t) n);
    }

    if (n < 0) {
        if (errno == EINTR) {
            printf("str_echo: catch EINTR, continue\n");
            goto again;
        }

        printf("str_echo: read error. errno: %d\n", errno);
    }
}

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child %d terminated\n", pid);
    }
    return;
}

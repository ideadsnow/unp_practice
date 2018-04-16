//
// Created by ids on 4/16/18.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/types/FILE.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include "../unp.h"

void str_cli(FILE *fp, int sockfd);


int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        printf("usage: tcpcli <IPaddress>\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);

    exit(0);
}

void str_cli(FILE *fp, int sockfd) {
    char sendline[MAXLINE], recvline[MAXLINE];
    ssize_t nread = 0;

    while (fgets(sendline, MAXLINE, fp) != NULL) {
        write(sockfd, sendline, strlen(sendline));

        bzero(&recvline, MAXLINE);
        if ((nread = read(sockfd, &recvline, MAXLINE)) < 0) {
            if (errno == EINTR) {
                printf("str_cli: Catch EINTR, continue");
                continue;
            }
            printf("str_cli: read error. errno: %d\n", errno);
            exit(0);
        } else if (nread == 0) {
            printf("str_cli: server terminated prematurely. errno: %d\n", errno);
            exit(0);
        } else {
            fputs(recvline, stdout);
        }
    }
}
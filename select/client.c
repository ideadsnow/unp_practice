//
// Created by ids on 18-4-16.
//

#include <sys/select.h>
#include <bits/types/FILE.h>

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

    Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);

    exit(0);
}

void str_cli(FILE *fp, int sockfd) {
    int maxfdp1, stdineof;
    fd_set rset;
    char buf[MAXLINE];
    ssize_t n;

    stdineof = 0;
    FD_ZERO(&rset);
    for (;;) {
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                if (stdineof == 1) {
                    return;
                } else {
                    printf("str_cli: server terminated prematurely. errno: %d\n", errno);
                    exit(1);
                }
            }
            write(fileno(stdout), buf, (size_t) n);
        }

        if (FD_ISSET(fileno(fp), &rset)) {
            if ((n = read(fileno(fp), buf, MAXLINE)) == 0) {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            writen(sockfd, buf, (size_t) n);
        }
    }
}
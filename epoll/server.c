//
// Created by ids on 18-4-16.
//

#include <sys/types.h>
#include <sys/epoll.h>
#include "../unp.h"

int main() {
    struct epoll_event ev, events[MAX_EVENTS];
    int i, listenfd, connfd, sockfd, epollfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    Listen(listenfd, SOMAXCONN);

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(1);
    }

    for (;;) {
        nready = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nready == -1) {
            perror("epoll_wait");
            exit(1);
        }

        for (i = 0; i < nready; i++) {
            // new client connection
            if (events[i].data.fd == listenfd) {
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

                // LT
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
                    perror("epoll_ctl: connfd");
                    exit(1);
                }

                if (--nready <= 0)
                    continue;
            } else {
                sockfd = events[i].data.fd;
                if ((n = read(sockfd, buf, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        close(sockfd);
                    } else {
                        printf("read error\n");
                        exit(1);
                    }
                } else if (n == 0) {
                    // connection closed by client. FIN received.
                    close(sockfd);
                } else
                    writen(sockfd, buf, (size_t) n);

                if (--nready <= 0)
                    break;
            }
        }
    }
}
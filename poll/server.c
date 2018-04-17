//
// Created by ids on 18-4-16.
//

#include <poll.h>
#include "../unp.h"

int main() {
    int i, maxi, listenfd, connfd, sockfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    Listen(listenfd, SOMAXCONN);

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;

    for (i = 1; i < OPEN_MAX; i++) {
        client[i].fd = -1;
    }
    maxi = 0;

    for (;;) {
        nready = poll(client, (nfds_t) maxi + 1, -1);

        // new client connection
        if (client[0].revents & POLLRDNORM) {
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

            for (i = 1; i < OPEN_MAX; i++)
                if (client[i].fd < 0) {
                    client[i].fd = connfd;
                    break;
                }

            if (i == OPEN_MAX) {
                printf("too many clients\n");
                exit(1);
            }

            client[i].events = POLLRDNORM;
            if (i > maxi)
                maxi = i;

            if (--nready <= 0)
                continue;
        }

        // check all clients for data
        for (i = 1; i <= maxi; i++) {
            if ((sockfd = client[i].fd) < 0)
                continue;
            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                if ((n = read(sockfd, buf, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        close(sockfd);
                        client[i].fd = -1;
                    } else {
                        printf("read error\n");
                        exit(1);
                    }
                } else if (n == 0) {
                    // connection closed by client. FIN received.
                    close(sockfd);
                    client[i].fd = -1;
                } else
                    writen(sockfd, buf, n);

                if (--nready <= 0)
                    break;
            }
        }
    }
}
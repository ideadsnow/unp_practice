//
// Created by ids on 18-4-17.
//

#include "unp.h"

void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
    if (bind(fd, sa, salen) < 0) {
        printf("bind error\n");
        exit(1);
    }
}


int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    int n;

    again:
    if ((n = accept(fd, sa, salenptr)) < 0) {
#ifdef    EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
            if (errno == ECONNABORTED)
#endif
            goto again;
        else
            printf("accept error\n");
    }
    return (n);
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
    if (connect(fd, sa, salen) < 0) {
        printf("connect error\n");
        exit(1);
    }
}

void Listen(int fd, int backlog) {
    if (listen(fd, backlog) < 0) {
        printf("listen error\n");
        exit(1);
    }
}
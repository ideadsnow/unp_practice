//
// Created by ids on 4/16/18.
//

#ifndef UNP_PRACTICE_UNP_H
#define UNP_PRACTICE_UNP_H

#define SERV_PORT 3333

#define MAXLINE 4096

#define OPEN_MAX 1024 // max open files

#define    min(a, b)    ((a) < (b) ? (a) : (b))
#define    max(a, b)    ((a) > (b) ? (a) : (b))

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <bits/types/FILE.h>
#include <limits.h>


ssize_t readn(int fd, void *vptr, size_t n);

ssize_t writen(int fd, const void *vptr, size_t n);

void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void Listen(int fd, int backlog);

#endif //UNP_PRACTICE_UNP_H

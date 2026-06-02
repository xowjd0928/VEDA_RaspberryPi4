#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dlfcn.h>

static void *clnt_connection(void *arg);
int sendData(FILE* fp, char *ct, char *filename);
void sendOk(FILE* fp);
void sendError(FILE* fp);
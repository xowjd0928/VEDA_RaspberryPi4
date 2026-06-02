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
#include <wiringPi.h>
#include <softPwm.h>

#define LED 1

/* 스레드 처리를 위한 함수 */
static void *clnt_connection(void *arg);
int sendData(FILE* fp, char *ct, char *filename);
void sendOk(FILE* fp);
void sendError(FILE* fp);

void* led_function(void* arg);
void* cds_function(void* arg);
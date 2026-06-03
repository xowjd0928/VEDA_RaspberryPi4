#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define SERVER_IP "100.82.123.25"
#define SERVER_PORT 60000

void send_request(const char* path);
int read_int();
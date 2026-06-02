#pragma once
#include <wiringPi.h>
#include <softTone.h>
#include <pthread.h>
#include <stdlib.h>

#define A 21
#define B 22
#define C 23
#define D 24
#define E 25
#define F 26
#define G 27
#define SPKR  6

void segment_function(int* arg);
void cleanup_function(void* arg);
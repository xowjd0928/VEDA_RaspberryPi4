#pragma once
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <softPwm.h>
#include <stdlib.h>

#define SW 5
#define CDS 0
#define LED 1

void cds_function(int* threshold);

#pragma once
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <softPwm.h>

#define	LED	1

void led_function(char* arg);
#pragma once
#include <wiringPi.h>
#include <softTone.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SPKR  6       /* GPIO25 */
#define TOTAL 32      /* 학교종의 전체 길이의 수 */
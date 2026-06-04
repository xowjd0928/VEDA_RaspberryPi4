#pragma once
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <softPwm.h>

// LED가 연결된 GPIO 번호
#define	LED	1

/**
 * @brief LED를 ON/OFF 및 밝기 조절하는 함수
 * @param arg LED 제어 문자열
 */
void led_function(char* arg);
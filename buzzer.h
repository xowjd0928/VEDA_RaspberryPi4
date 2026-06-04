#pragma once
#include <wiringPi.h>
#include <softTone.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 부저가 연결된 GPIO 핀 번호
#define SPKR  6

// 재생할 멜로디의 전체 음 개수
#define TOTAL 32

/**
 * @brief 부저를 제어하는 함수
 * 
 * 전달받은 제어 문자열에 따라 멜로디를 재생하거나
 * 현재 출력중인 소리를 중지한다.
 * 
 * @param arg 부저 제어 문자열
 */
void buzzer_function(char* arg);
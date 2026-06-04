#pragma once
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <softPwm.h>
#include <stdlib.h>

// CSD 스위치 핀 GPIO 번호
#define SW 5

// CDS 센서 GPIO 번호
#define CDS 0

// LED가 연결된 GPIO 번호
#define LED 1

/**
 * @brief CDS 센서 값을 주기적으로 측정하여 LED를 자동 제어하는 함수
 * 
 * 측정된 CDS 값과 Threshold 값을 비교하여 주변 밝기를 판단한다.
 * 밝은 환경에서는 LED를 끄고, 어두운 환경에서는 LED를 켠다.
 * 또한 현재 CDS 값과 Threshold 값을 로그 파일에 기록한다.
 * 
 * @param threshold CDS 임계값(Threshold)
 */
void cds_function(int* threshold);

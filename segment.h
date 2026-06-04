#pragma once
#include <wiringPi.h>
#include <softTone.h>
#include <pthread.h>
#include <stdlib.h>

// 7세그먼트 A~G GPIO 핀 번호
#define A 21
#define B 22
#define C 23
#define D 24
#define E 25
#define F 26
#define G 27

// 부저 핀 번호
#define SPKR  6

/**
 * @brief 카운트다운 타이머를 실행하는 함수
 *
 * 전달받은 숫자부터 0까지 1초 간격으로 카운트다운을 수행한다.
 * 카운트다운이 종료되면 부저를 울린다.
 *
 * @param arg 카운트다운 시작 번호
 */
void segment_function(int* arg);

/**
 * @brief 세그멘트 함수 종료 시 자원을 정리하는 함수
 *
 * 모든 세그먼트를 OFF 상태로 변경하고
 * 동적 할당된 메모리를 해제한다.
 *
 * @param arg 스레드 생성 인자
 */
void cleanup_function(void* arg);
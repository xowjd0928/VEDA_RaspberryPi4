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
#include <softTone.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/resource.h>

// 라즈베리파이 유저 홈 디렉토리
#define USER_HOME_PATH "/home/taejeong"

// LED가 연결된 GPIO 핀 번호
#define LED 1

// 부저가 연결된 GPIO 핀 번호
#define SPKR 6

// 7세그먼트 A~G 핀 번호
#define A 21
#define B 22
#define C 23
#define D 24
#define E 25
#define F 26
#define G 27

/**
 * @brief 프로세스를 데몬으로 전환한다.
 *
 * 부모 프로세스를 종료하고 세션을 생성하여
 * 백그라운드 서비스로 동작하도록 설정한다.
 *
 * @param arg 데몬화할 프로그램 이름
 */
void daemonize(char* arg);

/**
 * @brief WiringPi 및 하드웨어 장치를 초기화한다.
 */
void wiringpi_init();

/**
 * @brief 클라이언트의 HTTP 요청을 처리한다.
 *
 * 요청 URL에 따라 LED, CDS, Buzzer, Segment
 * 제어 기능을 수행하거나 파일을 전송한다.
 *
 * @param arg 클라이언트 소켓 디스크립터
 */
static void *clnt_connection(void *arg);

/**
 * @brief 요청된 파일을 클라이언트로 전송한다.
 *
 * @param fp 클라이언트 출력 스트림
 * @param ct Content-Type
 * @param filename 전송할 파일 이름
 * 
 * @return 0
 */
int sendData(FILE* fp, char *ct, char *filename);

/**
 * @brief HTTP 200 OK 응답을 전송한다.
 *
 * @param fp 클라이언트 출력 스트림
 */
void sendOk(FILE* fp);

/**
 * @brief HTTP 400 Bad Request 응답을 전송한다.
 *
 * @param fp 클라이언트 출력 스트림
 */
void sendError(FILE* fp);

/**
 * @brief LED 제어 스레드
 */
void* led_function(void* arg);

/**
 * @brief CSD 제어 스레드
 */
void* cds_function(void* arg);

/**
 * @brief 부저 제어 스레드
 */
void* buzzer_function(void* arg);

/**
 * @brief 7세그먼트 제어 스레드
 */
void* segment_function(void* arg);
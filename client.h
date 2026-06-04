/**
 * @file client.h
 * @brief 콘솔 클라이언트 인터페이스 (CLI)
 * @author taejeong
 * @date 2026-06-04
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

// 라즈베리파이 IP
#define SERVER_IP "100.82.123.25"

// 웹서버 포트 번호
#define SERVER_PORT 60000


/**
 * @brief SIGINT를 제외한 시그널들을 블록한다.
 */
void signal_block();

/**
 * @brief 콘솔 화면에 시스템 제어 메뉴판 인터페이스를 출력한다.
 */
void print_menu();

/**
 * @brief 표준 입력으로부터 안전하게 정수 데이터를 읽어온다.
 * @return 입력받은 정수값, 예외 발생 시 -1
 */
int read_int();

/**
 * @brief CDS 로그 기록을 위한 스레드를 초기화한다.
 */
void log_thread_init();

/**
 * @brief 백그라운드에서 주기적으로 CDS 로그를 기록하는 스레드 함수
 * @param arg 스레드 생성 인자
 * @return void* 스레드 종료 상태 포인터
 */
void* client_log_thread(void* arg);

/**
 * @brief 사용자가 선택한 메뉴 번호에 따라 제어 로직을 실행한다.
 * @param choice 선택한 메뉴 번호
 * @return 1을 반환하면 메인 루프 유지, 0을 반환하면 프로그램 종료
 */
int execute_menu(int choice);

/**
 * @brief 사용자로부터 임계값(Threshold)을 입력받아 CDS 기능을 요청한다.
 */
void handle_cds();

/**
 * @brief 사용자로부터 숫자를 입력받아 7세그멘트 타이머 기능을 요청한다.
 */
void handle_segment();

/**
 * @brief 지정된 URL 경로로 HTTP GET 요청 명령을 서버에 전송한다.
 * @param path 요청할 URL 경로
 */
void send_request(char* path);
#include "segment.h"

// 7세그멘트 핀 번호 배열
int pins[7] = {A, B, C, D, E, F, G};

// 숫자별 7세그멘트 출력 정보 배열
int digits[10][7] = {
    {0,0,0,0,0,0,1}, // 0
    {1,0,0,1,1,1,1}, // 1
    {0,0,1,0,0,1,0}, // 2
    {0,0,0,0,1,1,0}, // 3
    {1,0,0,1,1,0,0}, // 4
    {0,1,0,0,1,0,0}, // 5
    {0,1,0,0,0,0,0}, // 6
    {0,0,0,1,1,1,1}, // 7
    {0,0,0,0,0,0,0}, // 8
    {0,0,0,0,1,0,0}  // 9
};

/**
 * @brief 스레드 종료 시 세그멘트 상태를 초기화한다.
 *
 * 모든 세그멘트를 OFF 상태로 변경하고
 * 동적 할당된 메모리를 해제한다.
 *
 * @param arg 해제할 포인터 변수
 */
void cleanup_function(void* arg) {
    for (int i = 0; i < 7; i++) {
        digitalWrite(pins[i], HIGH);
    }
    free(arg);
}

/**
 * @brief 카운트다운 타이머를 실행한다.
 *
 * 전달받은 숫자부터 0까지 1초 간격으로 감소시키며
 * 7세그멘트에 표시한다.
 *
 * 카운트다운이 완료되면 부저음을 1초간 출력한다.
 *
 * @param arg 카운트다운 시작 번호
 */
void segment_function(int* arg) {
    pthread_cleanup_push(cleanup_function, arg);

    int num = *arg;

    // 카운트다운 
    for (int i = num; i >= 0; i--) {
        pthread_testcancel();
        for (int j = 0; j < 7; j++) {
            digitalWrite(pins[j], digits[i][j]);
        }
        delay(1000);
    }
    
    // 카운트 다운 완료 후 부저음 출력
    softToneWrite(SPKR, 2000);
    delay(1000);
    softToneWrite(SPKR, 0);

    pthread_cleanup_pop(1);
}
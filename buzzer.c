#include "buzzer.h"

// 학교 종을 위한 계이름
int notes[] = {
    391, 391, 440, 440, 391, 391, 329.63, 329.63,
    391, 391, 329.63, 329.63, 293.66, 293.66, 293.66, 0,
    391, 391, 440, 440, 391, 391, 329.63, 329.63,
    391, 329.63, 293.66, 329.63, 261.63, 261.63, 261.63, 0
};

/**
 * @brief 부저를 제어한다.
 *
 * ON 명령이 전달되면 학교종 멜로디를 재생한다.
 * OFF 명령이 전달되면 현재 출력 중인 소리를 중지한다.
 *
 * @param arg 부저 제어 문자열
 */
void buzzer_function(char* arg) {
    pthread_cleanup_push(free, arg);

    printf("buzzer_function: %s\n", arg);
    if (arg != NULL) {
        if (strcmp(arg, "ON") == 0) {
            // 학교 종 연주
            for (int i = 0; i < TOTAL; i++) {
                pthread_testcancel();
                softToneWrite(SPKR, notes[i]);
                delay(280);
            }
        } else if (strcmp(arg, "OFF") == 0) {
            softToneWrite(SPKR, 0);
        }
    }

    pthread_cleanup_pop(1);
}
#include "buzzer.h"

int notes[] = {       /* 학교종을 연주하기 위한 계이름 */
    391, 391, 440, 440, 391, 391, 329.63, 329.63,
    391, 391, 329.63, 329.63, 293.66, 293.66, 293.66, 0,
    391, 391, 440, 440, 391, 391, 329.63, 329.63,
    391, 329.63, 293.66, 329.63, 261.63, 261.63, 261.63, 0
};

void buzzer_function(char* arg) {
    pthread_cleanup_push(free, arg);
    printf("buzzer_function: %s\n", arg);
    if (arg != NULL) {
        if (strcmp(arg, "ON") == 0) {
            for (int i = 0; i < TOTAL; i++) {
                pthread_testcancel();
                softToneWrite(SPKR, notes[i]); /* 톤 출력: 학교종 연주 */
                delay(280);                    /* 음의 전체 길이만큼 출력되도록 대기 */
            }
        } else if (strcmp(arg, "OFF") == 0) {
            softToneWrite(SPKR, 0);
        }
    }
    pthread_cleanup_pop(1);
}
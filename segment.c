#include "segment.h"

int pins[7] = {A, B, C, D, E, F, G};

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

void cleanup_function(void* arg) {
    for (int i = 0; i < 7; i++) {
        digitalWrite(pins[i], HIGH);
    }
    free(arg);
}

void segment_function(int* arg) {
    pthread_cleanup_push(cleanup_function, arg);
    int num = *arg;

    for (int i = num; i >= 0; i--) {
        pthread_testcancel();
        for (int j = 0; j < 7; j++) {
            digitalWrite(pins[j], digits[i][j]);
        }
        delay(1000);
    }
    
    softToneWrite(SPKR, 2000);
    delay(1000);
    softToneWrite(SPKR, 0);
    pthread_cleanup_pop(1);
}
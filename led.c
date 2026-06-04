#include "led.h"

/**
 * @brief 제어 문자열에 따라 LED PWM 값을 변경한다.
 *
 * @param arg LED 제어 문자열
 */
void led_function(char* arg) {
    if (arg != NULL) {
        if (strcmp(arg, "OFF") == 0) {
            softPwmWrite(LED, 0);
        } else if (strcmp(arg, "LOW") == 0) {
            softPwmWrite(LED, 33);
        } else if (strcmp(arg, "MEDIUM") == 0) {
            softPwmWrite(LED, 66);
        } else if (strcmp(arg, "HIGH") == 0 || strcmp(arg, "ON") == 0) {
            softPwmWrite(LED, 100);
        }
    }
}
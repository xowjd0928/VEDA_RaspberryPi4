#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <softPwm.h>

#define	LED	1

void led_function(char* arg) {
    wiringPiSetup();

    softPwmCreate(LED, 0, 100);

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
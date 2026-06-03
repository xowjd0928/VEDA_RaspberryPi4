#include "cds.h"

void cds_function(int* threshold) {
    FILE* log = fopen("cds.log", "a");
    if (log == NULL) {
        perror("fopen cds.log");
        return;
    }
    int fd;
    int a2dChannel = 0;
    int prev, a2dVal;

    pthread_cleanup_push((void*)fclose, log);
    pthread_cleanup_push(free, threshold);
    fprintf(log, "[CDS Control] Current Threshold Value: %d\n", *threshold);
    fflush(log);

    if((fd = wiringPiI2CSetup(0x48)) < 0) {
        printf("wiringPiI2CSetup failed\n");
    }

    int cnt = 0;
    while(1) {
        pthread_testcancel();
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        wiringPiI2CWrite(fd, 0x00 | a2dChannel);
        prev = wiringPiI2CRead(fd);
        a2dVal = wiringPiI2CRead(fd);
        if (a2dVal < *threshold) {
            fprintf(log, "[%02d:%02d:%02d] Current CDS Value: %d (Bright)\n", t->tm_hour, t->tm_min, t->tm_sec, a2dVal);
            fflush(log);
            softPwmWrite(LED, 0);
        } else {
            fprintf(log, "[%02d:%02d:%02d] Current CDS Value: %d (Dark)\n", t->tm_hour, t->tm_min, t->tm_sec, a2dVal);
            fflush(log);
            softPwmWrite(LED, 100);
        }

        FILE* cur_log = fopen("current_cds.log", "w");
        if (cur_log != NULL) {
            fprintf(cur_log, "%d %d", a2dVal, *threshold);
            fclose(cur_log);
        }
        delay(1000);
        cnt++;
    }
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
}

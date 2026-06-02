#include "cds.h"

void cds_function(int* threshold) {
    FILE* log = fopen("cds.log", "a");
    int fd;
    int a2dChannel = 0;
    int prev, a2dVal;

    pthread_cleanup_push((void*)fclose, log);
    fprintf(log, "[CDS Control] Current Threshold Value: %d\n", a2dVal);
    fflush(log);

    if((fd = wiringPiI2CSetup(0x48)) < 0) {
        printf("wiringPiI2CSetup failed\n");
    }

    int cnt = 0;
    while(1) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        wiringPiI2CWrite(fd, 0x00 | a2dChannel);
        prev = wiringPiI2CRead(fd);
        a2dVal = wiringPiI2CRead(fd);
        if (a2dVal < *threshold) {
            fprintf(log, "[%02d:%02d:%02d] Current CDS Value: %d (Bright)\n", t->tm_hour, t->tm_min, t->tm_sec, a2dVal);
            fflush(log);
        } else {
            fprintf(log, "[%02d:%02d:%02d] Current CDS Value: %d (Dark)\n", t->tm_hour, t->tm_min, t->tm_sec, a2dVal);
            fflush(log);
        }
        delay(1000);
        cnt++;
    }
    pthread_cleanup_pop(1);
}

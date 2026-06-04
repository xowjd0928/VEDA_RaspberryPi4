#include "cds.h"

/**
 * @brief CDS 센서 값을 이용하여 LED를 자동 제어한다.
 *
 * CDS 센서 값을 1초마다 측정하고 Threshold 값과 비교하여
 * LED를 ON/OFF 한다.
 * 측정 결과는 cds.log에 저장되며
 * 가장 최근 측정값은 current_cds.log에 저장된다.
 *
 * @param threshold CDS 임계값
 */
void cds_function(int* threshold) {
    FILE* log = fopen("/tmp/cds.log", "a");
    if (log == NULL) {
        perror("fopen cds.log");
        return;
    }

    int fd;
    if((fd = wiringPiI2CSetup(0x48)) < 0) {
        printf("wiringPiI2CSetup failed\n");
    }

    int a2dChannel = 0;
    int prev, a2dVal;

    pthread_cleanup_push((void*)fclose, log);
    pthread_cleanup_push(free, threshold);

    fprintf(log, "[CDS Control] Current Threshold Value: %d\n", *threshold);
    fflush(log);

    while(1) {
        pthread_testcancel();

        wiringPiI2CWrite(fd, 0x00 | a2dChannel);
        prev = wiringPiI2CRead(fd);
        a2dVal = wiringPiI2CRead(fd);

        time_t now = time(NULL);
        struct tm* t = localtime(&now);

        // 시간 및 CDS 값 기록
        if (a2dVal < *threshold) {
            fprintf(log, "[%02d:%02d:%02d] Current CDS Value: %d (Bright)\n", t->tm_hour, t->tm_min, t->tm_sec, a2dVal);
            fflush(log);
            softPwmWrite(LED, 0);
        } else {
            fprintf(log, "[%02d:%02d:%02d] Current CDS Value: %d (Dark)\n", t->tm_hour, t->tm_min, t->tm_sec, a2dVal);
            fflush(log);
            softPwmWrite(LED, 100);
        }

        // client용 로그를 위한 최신 CDS 값 기록
        FILE* cur_log = fopen("/tmp/current_cds.log", "w");
        if (cur_log != NULL) {
            fprintf(cur_log, "%d %d", a2dVal, *threshold);
            fclose(cur_log);
        }

        delay(1000);
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
}

#include "client.h"

int read_int() {
    int value;
    char c;

    if (scanf("%d", &value) != 1) {
        while (getchar() != '\n'); // flush
        return -1;
    }

    if ((c = getchar()) != '\n' && c != EOF) {
        while (getchar() != '\n');
        return -1;
    }

    return value;
}


void send_request(const char* path) {
    int sock;
    struct sockaddr_in server;
    char request[1024];
    char buffer[4096];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    sprintf(request,
        "GET /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        path, SERVER_IP);

    send(sock, request, strlen(request), 0);

    int n;
    while ((n = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    close(sock);
}

void* client_log_thread(void* arg) {
    char buffer[1024];
    
    while(1) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            exit(1);
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            close(sock);
            perror("connect");
            exit(1);
        }

        char request[256];
        sprintf(request, "GET /get_cds_data HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", SERVER_IP);
        send(sock, request, strlen(request), 0);

        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            // HTTP 바디(데이터)만 추출
            char* body = strstr(buffer, "\r\n\r\n");
            if (body != NULL) {
                body += 4;
                
                int reading = 0, threshold = 0;
                sscanf(body, "%d,%d", &reading, &threshold);
                if (reading == 0 && threshold == 0) continue;

                // ⭐ client_cds.log 파일에 1초마다 기록 무한 누적
                FILE* log_fp = fopen("client_cds.log", "a");
                if (log_fp != NULL) {
                    time_t now = time(NULL);
                    struct tm* t = localtime(&now);
                    fprintf(log_fp, "[%02d:%02d:%02d] Current CDS: %d (Threshold: %d)\n", 
                            t->tm_hour, t->tm_min, t->tm_sec, reading, threshold);
                    fclose(log_fp);
                }
            }
        }
        close(sock);
        sleep(1); // 1초마다 반복 수행
    }
}

int main() {
    sigset_t sigset;

    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    pthread_t thread;
    if (pthread_create(&thread, NULL, client_log_thread, NULL)) {
        perror("pthread_create");
        exit(1);
    }
    pthread_detach(thread);

    int choice;

    while (1) {
        printf("\n==== MENU ====\n");
        printf("1. LED ON\n");
        printf("2. LED OFF\n");
        printf("3. LED LOW\n");
        printf("4. LED MEDIUM\n");
        printf("5. LED HIGH\n");
        printf("6. CDS Control\n");
        printf("7. CDS OFF\n");
        printf("8. BUZZER ON\n");
        printf("9. BUZZER OFF\n");
        printf("10. SEGMENT COUNTDOWN\n");
        printf("11. EXIT\n");
        printf(">> ");

        if ((choice = read_int()) == -1) {
            printf("Invalid input (number only)\n");
            continue;
        }

        switch (choice) {

            case 1:
                send_request("led_on");
                break;

            case 2:
                send_request("led_off");
                break;

            case 3:
                send_request("led_low");
                break;

            case 4:
                send_request("led_medium");
                break;

            case 5:
                send_request("led_high");
                break;

            case 6: {
                int threshold;
                char cds_path[64];

                printf("Enter CDS threshold: ");

                if ((threshold = read_int()) == -1) {
                    printf("Invalid input\n");
                    break;
                }

                sprintf(cds_path, "cds_start/%d", threshold);
                send_request(cds_path);
                break;
            }
            case 7:
                send_request("cds_off");
                break;

            case 8:
                send_request("buzzer_on");
                break;

            case 9:
                send_request("buzzer_off");
                break;

            case 10: {
                int num;
                char seg_path[64];

                printf("Enter number (0~9): ");

                if ((num = read_int()) == -1) {
                    printf("Invalid input\n");
                    break;
                }

                if (num < 0 || num > 9) {
                    printf("Only 0~9 allowed\n");
                    break;
                }

                sprintf(seg_path, "segment_start/%d", num);
                send_request(seg_path);
                break;
            }

            case 11:
                return 0;

            default:
                printf("invalid\n");
        }
    }

    return 0;
}
#include "client.h"

/**
 * @brief 메인 루프를 실행하며 사용자 입력을 받아 메뉴 명령을 수행한다.
 */
int main() {
    signal_block();
    log_thread_init();

    int choice;

    while (1) {
        print_menu();

        if ((choice = read_int()) == -1) {
            printf("Invalid input (number only)\n");
            continue;
        }

        if (!execute_menu(choice)) {
            break;
        }
    }
    printf("Bye!\n");
    return 0;
}

/**
 * @brief SIGINT를 제외한 시그널들을 블록한다.
 */
void signal_block() {
    sigset_t sigset;

    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);
    sigprocmask(SIG_SETMASK, &sigset, NULL);
}

/**
 * @brief 클라이언트용 CDS 로그 기록을 위한 스레드를 초기화한다.
 */
void log_thread_init() {
    pthread_t thread;
    if (pthread_create(&thread, NULL, client_log_thread, NULL)) {
        perror("pthread_create");
        exit(1);
    }
    pthread_detach(thread);
}

/**
 * @brief 1초 주기로 라즈베리파이 웹서버와 통신하여 실시간 CDS 값을 
 * 가져오고 CDS 로그 파일에 기록한다.
 * 
 * @param arg 스레드 생성 인자
 */
void* client_log_thread(void* arg) {
    char buffer[1024];
    
    while(1) {
        // 소켓 생성 및 서버 연결
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            sleep(1);
            continue;
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            close(sock);
            sleep(1);
            continue;
        }
        
        // HTTP GET 으로 CDS 데이터 요청
        char request[256];
        sprintf(request, "GET /get_cds_data HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", SERVER_IP);
        send(sock, request, strlen(request), 0);

        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            // HTTP 데이터 추출
            char* body = strstr(buffer, "\r\n\r\n");
            if (body != NULL) {
                body += 4;
                
                int reading = 0, threshold = 0;
                sscanf(body, "%d,%d", &reading, &threshold);

                // CDS Inactive시 무시
                if (reading == 0 && threshold == 0) {
                    close(sock);
                    sleep(1);
                    continue;
                }

                // 로그파일에 CDS 값 기록
                FILE* log = fopen("client_cds.log", "a");
                if (log != NULL) {
                    time_t now = time(NULL);
                    struct tm* t = localtime(&now);
                    fprintf(log, "[%02d:%02d:%02d] Current CDS: %d (Threshold: %d)\n", 
                            t->tm_hour, t->tm_min, t->tm_sec, reading, threshold);
                    fclose(log);
                }
            }
        }
        close(sock);
        sleep(1);
    }
}

/**
 * @brief 콘솔 화면에 시스템 제어 메뉴판 인터페이스를 출력한다.
 */
void print_menu() {
    printf("\n========== MENU ==========\n");
    printf("1. LED ON\n");
    printf("2. LED OFF\n");
    printf("3. LED LOW\n");
    printf("4. LED MED\n");
    printf("5. LED HIGH\n");
    printf("6. CDS START\n");
    printf("7. CDS STOP\n");
    printf("8. BUZZER ON\n");
    printf("9. BUZZER OFF\n");
    printf("10. TIMER START\n");
    printf("0. EXIT\n");
    printf("==========================\n");
    printf(">> ");
}

/**
 * @brief 표준 입력에서 정수만 반환하고 버퍼에 남아있는 문자를 제거한다.
 * 
 * @return 입력받은 정수값, 예외 발생 시 -1
 */
int read_int() {
    int num;
    char c;

    // 숫자만 입력받도록 설정
    if (scanf("%d", &num) != 1) {
        while (getchar() != '\n');
        return -1;
    }

    // 숫자 뒤에 남은 문자들을 제거
    if ((c = getchar()) != '\n') {
        while (getchar() != '\n');
        return -1;
    }

    return num;
}

/**
 * @brief 메뉴로부터 넘겨받은 사용자 선택지에 따라 제어 요청을 실행한다.
 * 
 * @param choice 사용자 선택 메뉴 번호
 */
int execute_menu(int choice) {
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

        case 6: 
            handle_cds();
            break;

        case 7:
            send_request("cds_off");
            break;

        case 8:
            send_request("buzzer_on");
            break;

        case 9:
            send_request("buzzer_off");
            break;

        case 10: 
            handle_segment();
            break;

        case 0:
            return 0;

        default:
            printf("invalid\n");
            break;
        }

    return 1;
}

/**
 * @brief CDS 구동을 위해 범위 확인(0~1023) 및 요청한다.
 */
void handle_cds() {
    int threshold;
    char cds_path[64];

    printf("Enter CDS threshold: ");

    if ((threshold = read_int()) == -1) {
        printf("Invalid input\n");
        return;
    }

    if (threshold < 0 || threshold > 1023) {
        printf("Only 0~1023 allowed\n");
        return;
    }

    sprintf(cds_path, "cds_start/%d", threshold);
    printf("Check client_cds.log!\n");
    send_request(cds_path);
}

/**
 * @brief 7세그먼트 타이머 구동을 위해 범위 확인(0~9) 및 요청한다.
 */
void handle_segment() {
    int num;
    char seg_path[64];

    printf("Enter number (0~9): ");

    if ((num = read_int()) == -1) {
        printf("Invalid input\n");
        return;
    }

    if (num < 0 || num > 9) {
        printf("Only 0~9 allowed\n");
        return;
    }

    sprintf(seg_path, "segment_start/%d", num);
    send_request(seg_path);
}

/**
 * @brief 라즈베리파이 웹서버와 소켓 통신을 열어 
 * 지정된 HTTP GET 을 요청하고 응답을 콘솔에 출력한다.
 */
void send_request(char* path) {
    int sock;
    struct sockaddr_in server;
    char request[1024];
    char buffer[4096];

    // 소켓 생성 및 서버 연결
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

    // HTTP GET 요청
    sprintf(request,
        "GET /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        path, SERVER_IP);

    send(sock, request, strlen(request), 0);

    // 응답 출력
    int n;
    n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        if (strstr(buffer, "200 OK")) {
            if (strstr(path, "led")) printf("LED Control Completed\n");
            else if (strstr(path, "cds")) printf("CDS Control Completed\n");
            else if (strstr(path, "buz")) printf("Buzzer Control Completed\n");
            else if (strstr(path, "seg")) printf("Segment Control Completed\n");
        } else {
            printf("Failed\n");
        }
    }

    close(sock);
    sleep(1);
}
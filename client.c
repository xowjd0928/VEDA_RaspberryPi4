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


int main() {
    sigset_t sigset;

    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    int choice;

    while (1) {
        printf("\n==== MENU ====\n");
        printf("1. LED ON\n");
        printf("2. LED OFF\n");
        printf("3. LED LOW\n");
        printf("4. LED MEDIUM\n");
        printf("5. LED HIGH\n");
        printf("6. CDS Control\n");
        printf("7. BUZZER ON\n");
        printf("8. BUZZER OFF\n");
        printf("9. SEGMENT COUNTDOWN\n");
        printf("10. EXIT\n");
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
                send_request("buzzer_on");
                break;

            case 8:
                send_request("buzzer_off");
                break;

            case 9: {
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

            case 10:
                return 0;

            default:
                printf("invalid\n");
        }
    }

    return 0;
}
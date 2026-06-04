#include "webserver.h"

pthread_t cds_thread;
pthread_t buzzer_thread;
pthread_t segment_thread;

/**
 * @brief 웹 서버를 실행한다.
 *
 * 서버 소켓을 생성하고 클라이언트 연결을 수락한 뒤
 * 요청 처리 스레드를 생성한다.
 *
 * @param argc 인자 개수
 * @param argv 인자 배열
 * 
 * @return 종료 코드
 */
int main(int argc, char **argv) {
    // 프로그램 시작 시 포트번호 받기
    if(argc!=2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    // 데몬화
    daemonize(argv[0]);

    // wiringPi 및 하드웨어 초기화
    wiringpi_init();

    int ssock;
    pthread_t thread;
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len;

    // 소켓 생성 및 서버 시작
    ssock = socket(AF_INET, SOCK_STREAM, 0);
    if(ssock == -1) {
        perror("socket()");
        return -1;
    }

    int optvalue = 1;
    setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = (argc != 2)?htons(8000):htons(atoi(argv[1]));
    // 서버 바인딩
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr))==-1) {
        perror("bind()");
        return -1;
    }
    
    // 최대 10개 클라이언트 연결 대기
    if(listen(ssock, 10) == -1) {
        perror("listen()");
        return -1;
    }

    while(1) {
        char mesg[BUFSIZ];
        int csock;

        // 클라이언트 요청 대기
        len = sizeof(cliaddr);
        csock = accept(ssock, (struct sockaddr*)&cliaddr, &len);

        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client IP : %s:%d\n", mesg, ntohs(cliaddr.sin_port));

        // 클라이언트 요청 처리
        pthread_create(&thread, NULL, clnt_connection, &csock);
        pthread_join(thread, NULL);
    }
    return 0;
}
/**
 * @brief 현재 프로세스를 데몬 프로세스로 전환한다.
 *
 * 세션을 생성하고 표준 입출력을 제거하여
 * 백그라운드 서비스 형태로 실행한다.
 *
 * @param arg 프로그램 이름
 */
void daemonize(char* arg) {
    struct sigaction sa;
    struct rlimit rl;
    int fd0, fd1, fd2, i;
    pid_t pid;

    // 파일 생성을 위한 마스크를 0으로 설정
    umask(0);

    // 사용할 수 있는 최대의 파일 디스크립터 수 얻기
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("getlimit()");
    }

    // ppid 1로 고정
    if((pid = fork()) < 0) {
        perror("error()");
    } else if(pid != 0) {
        exit(0);
    }

    // 세션 리더 되기
    setsid();

    // 시그널 무시
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("sigaction() : Can't ignore SIGHUP");
    }

    // 프로세스의 워킹 디렉터리를 홈으로 설정한다
    if(chdir(HOME_PATH) < 0) {
        perror("cd()");
    }

    // 프로세스의 모든 파일 디스크립터 닫기
    if(rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }

    for(i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    // 파일 디스크립터 0, 1, 2를 /dev/null로 연결
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    // 로그 출력을 위한 파일 로그 열기
    openlog(arg, LOG_CONS, LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }

    // 로그 파일에 정보 수준의 로그를 출력
    syslog(LOG_INFO, "Daemon Process Initialized");
}
/**
 * @brief GPIO 및 주변 장치를 초기화한다.
 *
 * LED, Segment, Buzzer 장치 사용을 위한
 * 핀 모드와 PWM/Tone 기능을 설정한다.
 */
void wiringpi_init() {
    wiringPiSetup();
    pinMode(LED, OUTPUT);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);
    pinMode(E, OUTPUT);
    pinMode(F, OUTPUT);
    pinMode(G, OUTPUT);
    softPwmCreate(LED, 0, 100);
    softToneCreate(SPKR);
    remove("current_cds.log");
}
/**
 * @brief 클라이언트 HTTP 요청을 처리한다.
 *
 * URL에 따라 장치를 제어하거나
 * 파일을 클라이언트에게 전송한다.
 *
 * @param arg 클라이언트 소켓 디스크립터
 */
void *clnt_connection(void *arg) {
    // 스레드를 통해서 넘어온 arg를 int 형의 파일 디스크립터로 변환한다
    int csock = *((int*)arg);
    FILE *clnt_read, *clnt_write;
    char reg_line[BUFSIZ], reg_buf[BUFSIZ];
    char method[BUFSIZ], type[BUFSIZ];
    char filename[BUFSIZ], *ret;

    // 파일 디스크립터를 FILE 스트림으로 변환한다.
    clnt_read = fdopen(csock, "r");
    clnt_write = fdopen(dup(csock), "w");

    // 한 줄의 문자열을 읽어서 reg_line 변수에 저장한다.
    fgets(reg_line, BUFSIZ, clnt_read);
    
    // reg_line 변수에 문자열을 화면에 출력한다.
    fputs(reg_line, stdout);

    // ' ' 문자로 reg_line을 구분해서 요청 라인의 내용(메소드)를 분리한다.
    ret = strtok(reg_line, "/ ");
    strcpy(method, (ret != NULL)?ret:"");
    if(strcmp(method, "POST") == 0) { 		// POST 메소드일 경우를 처리한다.
        sendOk(clnt_write); 			// 단순히 OK 메시지를 클라이언트로 보낸다.
        goto END;
    } else if(strcmp(method, "GET") != 0) {	// GET 메소드가 아닐 경우를 처리한다.
        sendError(clnt_write); 			// 에러 메시지를 클라이언트로 보낸다.
        goto END;
    }

    ret = strtok(NULL, " "); 			// 요청 라인에서 경로(path)를 가져온다.
    strcpy(filename, (ret != NULL)?ret:"");
    if(filename[0] == '/') { 			// 경로가 '/'로 시작될 경우 /를 제거한다.
        for(int i = 0, j = 0; i < BUFSIZ; i++, j++) {
            if(filename[0] == '/') j++;
            filename[i] = filename[j];
            if(filename[j] == '\0') break;
        }
    }

    // 디폴트 페이지를 index로 설정한다.
    if (strlen(filename) == 0) {
        strcpy(filename, "index.html");
    }
    
    // 각각의 HTTP GET 요청에 맞는 함수를 실행한다.
    if (strcmp(filename, "led_on") == 0) {
        if (cds_thread != 0) {
            pthread_cancel(cds_thread);
            pthread_join(cds_thread, NULL);
        }
        pthread_t thread;
        char* arg = strdup("ON");
        if (pthread_create(&thread, NULL, led_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        pthread_join(thread, NULL);
        free(arg);
        sendOk(clnt_write);
        goto END;
    } else if (strcmp(filename, "led_off") == 0) {
        if(cds_thread != 0) {
            pthread_cancel(cds_thread);
            pthread_join(cds_thread, NULL);
        }
        pthread_t thread;
        char* arg = strdup("OFF");
        if (pthread_create(&thread, NULL, led_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        pthread_join(thread, NULL);
        free(arg);
        sendOk(clnt_write);
        goto END;
    } else if (strcmp(filename, "led_low") == 0) {
        if(cds_thread != 0) {
            pthread_cancel(cds_thread);
            pthread_join(cds_thread, NULL);
        }
        pthread_t thread;
        char* arg = strdup("LOW");
        if (pthread_create(&thread, NULL, led_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        pthread_join(thread, NULL);
        free(arg);
        sendOk(clnt_write);
        goto END;
    } else if (strcmp(filename, "led_medium") == 0) {
        if (cds_thread != 0) {
            pthread_cancel(cds_thread);
            pthread_join(cds_thread, NULL);
        }
        pthread_t thread;
        char* arg = strdup("MEDIUM");
        if (pthread_create(&thread, NULL, led_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        pthread_join(thread, NULL);
        free(arg);
        sendOk(clnt_write);
        goto END;
    } else if (strcmp(filename, "led_high") == 0) {
        if (cds_thread != 0) {
            pthread_cancel(cds_thread);
            pthread_join(cds_thread, NULL);
        }
        pthread_t thread;
        char* arg = strdup("HIGH");
        if (pthread_create(&thread, NULL, led_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        pthread_join(thread, NULL);
        free(arg);
        sendOk(clnt_write);
        goto END;
    } 
    else if (strncmp(filename, "cds_start/", 10) == 0) {
        if (cds_thread != 0) {
            pthread_cancel(cds_thread);
            pthread_join(cds_thread, NULL);
        }
        int* threshold = malloc(sizeof(int));
        *threshold = atoi(filename + 10);
        if (pthread_create(&cds_thread, NULL, cds_function, threshold)) {
            perror("pthread_create");
            exit(1);
        }
        sendOk(clnt_write);
        goto END;
    } else if (strcmp(filename, "cds_off") == 0) {
        if (cds_thread != 0) {
            pthread_cancel(cds_thread);
            pthread_join(cds_thread, NULL);
        }
        softPwmWrite(LED, 0);
        remove("current_cds.log");
        sendOk(clnt_write);
        goto END;
    } else if (strcmp(filename, "get_cds_data") == 0) {
        int reading = 0, threshold = 0;
        int file_exists = 0;
        
        FILE* cur_log = fopen("current_cds.log", "r");
        if(cur_log != NULL) {
            fscanf(cur_log, "%d %d", &reading, &threshold);
            fclose(cur_log);
            file_exists = 1;
        }

        char response_header[] = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n\r\n";
            
        char body[64];
        if (file_exists) {
            sprintf(body, "%d,%d", reading, threshold);
        } else {
            sprintf(body, "0,0");
        }
        
        fputs(response_header, clnt_write);
        fputs(body, clnt_write);
        fflush(clnt_write);
        goto END;
    } else if (strcmp(filename, "buzzer_on") == 0) {
        if (buzzer_thread != 0) {
            pthread_cancel(buzzer_thread);
            pthread_join(buzzer_thread, NULL);
        }
        char* arg = strdup("ON");
        if (pthread_create(&buzzer_thread, NULL, buzzer_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        sendOk(clnt_write);
        goto END;
    } else if (strcmp(filename, "buzzer_off") == 0) {
        if (buzzer_thread != 0) {
            pthread_cancel(buzzer_thread);
            pthread_join(buzzer_thread, NULL);
        }
        char* arg = strdup("OFF");
        if (pthread_create(&buzzer_thread, NULL, buzzer_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        pthread_join(buzzer_thread, NULL);
        sendOk(clnt_write);
        goto END;
    } else if (strncmp(filename, "segment_start/", 14) == 0) {
        int num = atoi(filename + 14);

        int* arg = malloc(sizeof(int));
        *arg = num;
        if (segment_thread != 0) {
            pthread_cancel(segment_thread);
            pthread_join(segment_thread, NULL);
        }
        if (pthread_create(&segment_thread, NULL, segment_function, arg)) {
            perror("pthread_create");
            exit(1);
        }
        sendOk(clnt_write);
        goto END;
    }

    // 메시지 헤더를 읽어서 화면에 출력하고 나머지는 무시한다.
    do {
        fgets(reg_line, BUFSIZ, clnt_read);
        fputs(reg_line, stdout);
        strcpy(reg_buf, reg_line);
        char* buf = strchr(reg_buf, ':');
    } while(strncmp(reg_line, "\r\n", 2)); 	// 요청 헤더는 ‘\r\n’으로 끝난다.

    // 파일의 이름을 이용해서 클라이언트로 파일의 내용을 보낸다.
    sendData(clnt_write, type, filename);

END:
    fclose(clnt_read); 				// 파일의 스트림을 닫는다.
    fclose(clnt_write);
    pthread_exit(0); 				// 스레드를 종료시킨다.

    return (void*)NULL;
}
/**
 * @brief 파일을 클라이언트로 전송한다.
 *
 * HTML 또는 CSS 파일을 읽어 HTTP 응답으로 전달한다.
 *
 * @param fp 클라이언트 출력 스트림
 * @param ct Content-Type
 * @param filename 파일 이름
 * 
 * @return 0
 */
int sendData(FILE* fp, char *ct, char *filename) {
    // 클라이언트로 보낼 성공에 대한 응답 메시지
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server:Netscape-Enterprise/6.0\r\n";
    char cnt_type[ ] = "Content-Type:text/html\r\n";
    char end[ ] = "\r\n"; 			// 응답 헤더의 끝은 항상 \r\n
    char buf[BUFSIZ];
    int fd, len;

    if (strstr(filename, ".css") != NULL) {
        strcpy(cnt_type, "Content-Type:text/css\r\n");
    } else {
        strcpy(cnt_type, "Content-Type:text/html\r\n");
    }

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_type, fp);
    fputs(end, fp);

    fd = open(filename, O_RDWR); 		// 파일을 연다.
    do {
        len = read(fd, buf, BUFSIZ); 		// 파일을 읽어서 클라이언트로 보낸다.
        fputs(buf, fp);
    } while(len == BUFSIZ);

    close(fd); 					// 파일을 닫는다.

    return 0;
}

/**
 * @brief HTTP 200 OK 응답을 전송한다.
 *
 * @param fp 클라이언트 출력 스트림
 */
void sendOk(FILE* fp) {
    // 클라이언트에 보낼 성공에 대한 HTTP 응답 메시지
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0\r\n\r\n";

    fputs(protocol, fp);
    fputs(server, fp);
    fflush(fp);
}

/**
 * @brief HTTP 400 Bad Request 응답을 전송한다.
 *
 * @param fp 클라이언트 출력 스트림
 */
void sendError(FILE* fp) {
    // 클라이언트로 보낼 실패에 대한 HTTP 응답 메시지
    char protocol[ ] = "HTTP/1.1 400 Bad Request\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0\r\n";
    char cnt_len[ ] = "Content-Length:1024\r\n";
    char cnt_type[ ] = "Content-Type:text/html\r\n\r\n";

    // 화면에 표시될 HTML의 내용
    char content1[ ] = "<html><head><title>BAD Connection</title></head>";
    char content2[ ] = "<body><font size=+5>Bad Request</font></body></html>";
    printf("send_error\n");

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    fputs(content1, fp);
    fputs(content2, fp);
    fflush(fp);
}

/**
 * @brief LED 제어 함수를 동적 라이브러리에서 호출한다.
 *
 * @param arg LED 제어 문자열
 */
void* led_function(void* arg) {
    void* handle;
    void (*fptr)(char*);
    char* error;
    handle = dlopen("./librasp.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();

    fptr = dlsym(handle, "led_function");
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    fptr((char*)arg);

    return(NULL);
}

/**
 * @brief CDS 제어 함수를 동적 라이브러리에서 호출한다.
 *
 * @param arg 임계값(Threshold)
 */
void* cds_function(void* arg) {
    void* handle;
    void (*fptr)(int*);
    char* error;

    handle = dlopen("./librasp.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();

    fptr = dlsym(handle, "cds_function");
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    fptr((int*)arg);

    return(NULL);
}

/**
 * @brief 부저 제어 함수를 동적 라이브러리에서 호출한다.
 *
 * @param arg 부저 제어 문자열
 */
void* buzzer_function(void* arg) {
    void* handle;
    void (*fptr)(char*);
    char* error;

    handle = dlopen("./librasp.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();

    fptr = dlsym(handle, "buzzer_function");
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    fptr((char*)arg);

    return(NULL);
}

/**
 * @brief 세그먼트 함수를 동적 라이브러리에서 호출한다.
 *
 * @param arg 카운트다운 시작 번호
 */
void* segment_function(void* arg) {
    void* handle;
    void (*fptr)(int*);
    char* error;

    handle = dlopen("./librasp.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();

    fptr = dlsym(handle, "segment_function");
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    fptr((int*)arg);

    return(NULL);
}
# Raspberry Pi Hardware Control Server

## 개요

라즈베리파이4에서 동작하는 하드웨어 제어 서버이다.

다음 장치를 제어할 수 있다.

- LED ON/OFF 및 밝기 조절
- CDS 조도 센서 기반 LED 자동 제어
- 부저(Buzzer) 재생
- 7-Segment 카운트다운 타이머

사용자는 다음 두 가지 방식으로 서버를 제어할 수 있다.

1. CLI(Client Program)
2. Web UI(Browser)

---

# 실행 환경

## 서버

- Raspberry Pi 4

## 클라이언트

- Ubuntu Linux

---

# 사전 설정

## 1. Makefile 수정

Makefile 내부의 USER 및 HOST 값을 자신의 환경에 맞게 수정한다.

예시:

```make
USER = taejeong
HOST = 100.82.123.25
```

- USER : Raspberry Pi 로그인 계정
- HOST : Raspberry Pi IP 주소

---

## 2. client.h 수정

client.h 내부의 SERVER_IP 및 SERVER_PORT 값을 수정한다.

예시:

```c
#define SERVER_IP "100.82.123.25"
#define SERVER_PORT 60000
```

- SERVER_IP : Raspberry Pi 서버 IP
- SERVER_PORT : 서버 포트 번호

---

## 3. webserver.h 수정

webserver.h 내부의 USER_HOME_PATH 값을 수정한다.

예시:

```c
#define USER_HOME_PATH "/home/taejeong"
```

- USER_HOME_PATH : Raspberry Pi 홈 디렉토리


# 빌드

Ubuntu 터미널에서 다음 명령어를 실행한다.

```bash
make
```

---

# 서버 실행

라즈베리파이 터미널에서 다음 명령어를 실행한다.

```bash
./webserver SERVER_PORT
```

예시:

```bash
./webserver 60000
```

정상적으로 실행되면 서버가 클라이언트 연결을 대기한다.

---

# 사용 방법

사용자는 CLI 또는 Web UI 중 하나를 선택하여 사용할 수 있다.

---

# 방법 1 : CLI 사용

Ubuntu 터미널에서 다음 명령어를 실행한다.

```bash
./client
```

메뉴가 출력되며 원하는 기능을 선택하여 장치를 제어할 수 있다.

예시:

```text
========== MENU ==========
1. LED ON
2. LED OFF
3. LED BRIGHT
4. LED MIDDLE
5. LED DIM
6. CDS START
7. CDS STOP
8. BUZZER ON
9. BUZZER OFF
10. TIMER START
0. EXIT
==========================
```

메뉴 번호를 입력하여 기능을 실행한다.

---

# 방법 2 : Web UI 사용

웹 브라우저를 열고 다음 주소로 접속한다.

```text
http://<SERVER_IP>:<SERVER_PORT>
```

예시:

```text
http://100.82.123.25:60000
```

접속 후 웹 페이지에서 제공하는 버튼 및 입력창을 이용하여 장치를 제어할 수 있다.

---

# 로그 파일

프로그램 실행 중 다음 로그 파일이 생성될 수 있다.

## 서버 로그 파일

```text
cds.log
current_cds.log
```

### cds.log

CDS 센서의 동작 로그를 기록한다.

예시:

```text
[CDS Control] Current Threshold Value: 300
[12:31:05] Current CDS Value: 250 (Bright)
[12:31:06] Current CDS Value: 480 (Dark)
```

### current_cds.log

가장 최근 CDS 상태를 저장한다.

형식:

```text
<현재 CDS 값> <Threshold 값>
```

예시:

```text
425 300
```

---

## 클라이언트 로그 파일

```text
client_cds_log
```

예시:

```text
[12:31:08] Current CDS: 220 (Threshold: 180)
```

### client_cds.log

CDS 센서의 동작 로그를 클라이언트가 볼 수 있게 기록한다.


# 종료 방법

CLI 사용 시:

```text
0
```

혹은

```bash
Ctrl + C
```

을 입력하여 종료한다.

서버 종료 시:

```bash
ps -ef | grep webserver
```
입력 후

```bash
kill <webserver pid>
```

를 입력한다.

예시: 

```bash
ps -ef | grep webserver
taejeong   17796       1  2 09:20 ?        00:01:17 ./webserver 60000
```

```bash
kill 17796
```

---

### 코딩 스타일 및 규칙

본 프로젝트는 하단의 코딩 스타일 가이드를 따른다.

스타일 가이드에는 다음 내용이 포함되어 있다.

- 브랜치 네이밍 규칙
- 코드 포맷팅 규칙
- 주석 작성 규칙
- 식별자 명명 규칙
- HTML / CSS / JavaScript 작성 규칙

자세한 내용은 아래 문서를 참고한다.

[STYLE.md](STYLE.md)

 

# 스타일 가이드 (Style Guide)

## 일반 지침 (General Guidelines)

- 모든 식별자는 특별한 경우를 제외하고 `a-z`, `A-Z`, `0-9`, `_` 문자만 사용한다.
- 파일 간 일관성을 유지한다.
- 사용하지 않는 변수, 함수, 헤더는 제거한다.
- 동일한 기능에 대해 서로 다른 스타일을 혼용하지 않는다.

---

## Git 브랜치 규칙

브랜치 형식은 다음과 같다.

```text
<sourcecodename>/<category>/<purpose>
```

예시:

```text
led/feat/on_and_off
```

관련된 소스 코드가 여러 개인 경우 첫 번째 파트는 생략할 수 있다.

```text
bug/fix_memory_leak
```

### 카테고리 종류

| 카테고리 | 설명            |
| ---- | ------------- |
| feat | 기능 추가, 제거, 수정 |
| doc  | 문서 추가, 제거, 수정 |
| bug  | 버그 수정         |
| test | 테스트           |

---

## 코드 포맷팅 (Code Formatting)

### 들여쓰기 (Indentation)

- 들여쓰기는 공백(Space) 4칸을 사용한다.

---

### 중괄호 및 공백 (Braces & Spacing)

중괄호는 K&R 스타일을 사용한다.

```c
if (condition) {
    do_something();
} else {
    do_something_else();
}
```

---

### 줄 길이 (Line Length)

- 지나치게 긴 한 줄 코드는 지양한다.
- 가독성이 향상되는 경우 적절히 줄바꿈한다.

---

### 주석 작성 규칙 (Comments)

- **데이터 구조 멤버**: 해당 필드의 기능 및 목적을 설명한다.
- 짧은 설명 또는 구현 관련 주석은 단일 라인 주석(`//` 또는 `#`)을 사용한다.
- 긴 설명(여러 줄, 여러 문단, 콘솔 출력 예시, 다이어그램 등)은 다중 라인 주석(`/* ... */` 또는 `''' ... '''`)을 사용한다.
- 클래스/메서드 바로 위에는 인터페이스 주석으로 다중 라인 주석을 사용하며, JavaDoc / Doxygen / Sphinx 등 문서 스타일을 따른다.

```c
/**
 * @brief 주어진 점수가 현재 최고 점수보다 높은지 확인한다.
 *
 * @param new_score 현재 최고 점수와 비교할 점수
 * @return 1이면 새로운 점수가 더 높음, 아니면 0
 */
int is_new_highscore (int new_score) {
    return new_score > high_score;
}
```

---

## 식별자 명명 규칙 (Identifier Naming Conventions)

### C 언어

#### 변수

`snake_case`를 사용한다.

```c
int threshold_value;
char client_ip[32];
```

#### 함수

`snake_case`를 사용한다.

```c
void set_threshold(void);
int get_sensor_value(void);
```

#### 상수 및 매크로

`UPPER_CASE`를 사용한다.

```c
#define MAX_CLIENTS 20
#define SERVER_PORT 8080
```

#### 구조체 타입

`snake_case_t`를 사용한다.

```c
typedef struct {
    int value;
} sensor_data_t;
```

---

## 언어별 규칙 (Language Specific Guidelines)

### HTML

#### ID

`kebab-case`를 사용한다.

```html
<div id="cds-status"></div>
<button id="btn-timer-submit"></button>
```

#### 속성(Attribute)

* 속성 값은 큰따옴표(`"`)를 사용한다.
* 동일한 형태의 태그는 속성 순서를 일관성 있게 유지한다.

예시:

```html
<input
    type="number"
    id="threshold"
    class="input-field"
    min="0"
    max="1023"
>
```

---

### CSS

#### 클래스 이름

`kebab-case`를 사용한다.

```css
.input-field { }
.input-with-icon { }
.icon-wrapper { }
.btn-primary { }
.status-active { }
```

---

### JavaScript

#### 변수

`camelCase`를 사용한다.

```javascript
const thresholdValue = 100;
let currentReading = 0;
```

#### 함수

`camelCase`를 사용한다.

```javascript
function setThreshold() {
    ...
}

function updateStatus() {
    ...
}
```

#### 상수

`UPPER_CASE`를 사용한다.

```javascript
const MAX_THRESHOLD = 1023;
const API_TIMEOUT = 5000;
```

---

## 파일 명명 규칙

- 모든 소스 파일은 `snake_case`를 사용한다.

```text
web_server.c
segment.c
```

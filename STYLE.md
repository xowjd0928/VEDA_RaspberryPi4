# 스타일 (Styles)

## 일반 지침 (General Guidelines)
- 모든 이름은 `a-z`, `A-Z`, `0-9`, `_` 문자만 사용한다.
- 파일 간 일관성을 유지한다.

---

## 깃 브랜치 (Branches)

- 브랜치 포맷팅 : `<sourcecodename>/<category>/<purpose>` Ex: `led/feat/on_and_off`

- 관련된 소스코드가 많을 경우 첫번째 파트는 무시 Ex: `bug/fix_mem_leak`

- `<category>` 종류:
    - `feat`: 기능 추가/제거/수정
    - `doc`: 문서 추가/제거/수정
    - `bug`: 버그 수정
    - `test`: 테스트 브랜치

---

## 코드 포맷팅 (Code Formatting)

### 들여쓰기 (Indentation)
- 탭은 **4칸 공백**으로 사용한다.

---

### 중괄호 & 공백 (Braces & Spacing)
- 중괄호는 **K&R 스타일**을 사용한다:

```c
if (condition) {
    doSomething();
} else {
    doSomethingElse();
}
```

---

### 주석 (Comments)

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

- **변수**: `snake_case` 사용 (예: `first_name`)
- **함수**: `snake_case` 사용 (예: `second_name`)
- **상수**: `UPPER_CASE` 사용 (예: `MAX_RETRIES`)
```

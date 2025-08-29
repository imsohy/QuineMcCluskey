# Quine-McCluskey Algorithm Implementation  
---
## 👥 Authors

김동욱 · 최경정 · 김민경 · 장현웅

---
[![Demo Video](https://img.youtube.com/vi/jBF4zhkyvls/0.jpg)](https://youtu.be/jBF4zhkyvls)  

---

## 🔎 프로젝트 개요
디지털 시스템 설계에서 **논리식 최소화(Boolean Minimization)**는 성능과 비용을 동시에 좌우하는 핵심 과제입니다.  
동일한 기능의 회로라도 표현이 단순할수록 **게이트 수/트랜지스터 수가 줄고**, 이는 곧 **칩 면적, 소비 전력, 제조 비용** 절감으로 이어집니다. 
이 프로젝트는 이러한 목적을 위해, **Quine–McCluskey 알고리즘(Q–M)**을 **C++**로 구현하여, 
주어진 불 대수(Boolean Function)를 **자동으로 최소 SOP(Sum of Products) 형태**로 변환하고 **회로 비용(트랜지스터 수)**까지 계산합니다.

본 구현은 다음을 수행합니다:
- 입력으로 **minterm(`m`)**과 **don't care(`d`)**를 받아,  
- 반복적인 **항 결합(reduce)**으로 **Prime Implicant(PI)**를 도출하고,  
- **Essential Prime Implicant(EPI)**를 식별한 뒤,  
- 남은 항을 가장 적은 수의 PI로 덮는 **탐욕적(그리디) 선택**을 통해  
- **최소화된 SOP 표현과 비용(Cost)**을 출력합니다.

---
## 🎯 구현 목표

- **임의 개수의 입력 변수를 가진 불 대수 함수**에 대해, Quine–McCluskey(Q–M) 알고리즘으로
  **최소 SOP(Sum of Products) 표현**을 자동 산출한다.

- 과제 명세의 **입출력 규격**을 따른다:  
  `input_minterm.txt`(첫 줄: 비트 길이, 이후 각 줄: `m|d` + 2진수) →  
  `result.txt`(선택된 PI 목록 + 최종 비용 `Cost (# of transistors): N`) 출력.

- **Prime Implicant(PI) 도출**: 해밍 거리 1인 항을 `-`로 결합하는 **reduce 루프**를 반복하여
  더 이상 축약되지 않을 때까지 PI를 생성한다. 축약에 참여하지 못한 항은 PI로 수집한다.

- **Essential Prime Implicant(EPI) 식별**:  
  PI→minterm, minterm→PI **양방향 매핑 테이블**을 구성하고, 특정 minterm을 **유일하게 덮는 PI**를
  필수 항(EPI)으로 선택하여 해당 minterm을 제거한다.

- **잔여 커버링 최소화**: 남은 minterm을 가장 많이 덮는 PI부터 선택하는 **그리디 전략**으로
  모든 minterm을 커버하는 최소 조합을 완성한다.  
  (*참고: Petrick’s Method는 미적용 — 단순성과 실행 성능을 우선.*)

- **하드웨어 비용 산출**: 최종 SOP를 **트랜지스터 개수**로 환산해 `result.txt`에 기재한다.  
  구현식:  
  `transistors = 2*MINTERM_LENGTH  +  Σ(2*bit + 2 over PIs)  +  (2*|PIs| + 2)`  
  (입력 인버터 + AND 게이트 + OR 게이트)

- **예외 및 범용성 보장**: don’t care만 존재하는 경우, 모든 항이 참인 경우, 입력 비트 수가 달라지는 경우 등
  **다양한 테스트 케이스로 검증**한다.

---
## 📥 입출력 형식 (File I/O)

- 파일 구조
```bash
├─ QuineMcklusky.cpp                 # C++ 구현
├─ input_minterm.txt                 # 입력 예시
├─ result.txt                        # 결과(실행 후 생성)
└─ Implementation of Quine-McCluskey Algorithm.pdf  # 보고서
```

- **입력 파일**: `input_minterm.txt`
<비트길이>
<m: minterm|d:don't care value> <2진수>
...

예)
```bash
4
d 0000
m 0100
m 0101
...
```

- **출력 파일**: `result.txt`

<선택된 PI 한 줄씩>
Cost (# of transistors): <정수>

---
## ▶️ Demo Build & Run

```bash
# Compile
g++ QuineMcklusky.cpp -O2 -std=c++17 -o quine

# Run (input_minterm.txt와 동일 디렉터리)
./quine

# 결과 확인
cat result.txt
```

---
✅ 검증 사례 (예시)

입력 (input_minterm.txt에 추가)
```bash
4
d 0000
m 0100
m 0101
m 0110
m 1001
m 1010
d 0111
d 1101
d 1111
```

출력

```bash
01--
1-01
1010

Cost (# of transistors): 40
```



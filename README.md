# Quine-McCluskey Algorithm Implementation  

2022-2 광운대학교 컴퓨터정보공학부 디지털논리회로1 중간과제

---
## Authors

김동욱 · 최경정 · 김민경 · 장현웅

---
[![Demo Video](https://img.youtube.com/vi/jBF4zhkyvls/0.jpg)](https://youtu.be/jBF4zhkyvls)  
---
## 프로젝트 개요
디지털 시스템 설계에서 논리식 최소화(Boolean Minimization)는 성능과 비용을 동시에 좌우하는 핵심 과제이다.
동일한 기능의 회로라도 표현이 단순할수록 게이트 수/트랜지스터 수가 줄고, 이는 곧 칩 면적, 소비 전력, 제조 비용 절감으로 이어진다.
예를 들어, 다음 A,B의 논리 부울 식을 최소화하면 

(A,B에 0/1의 값이 들어감, `: NOT 표현)

F1(A,B,C)=AB+A′B+AB′

최소 SOP: A+B

로, 연산을 수행하는데 필요한 게이트 수가 줄어든다.

이 프로젝트는 이러한 목적을 위해, Quine–McCluskey 알고리즘(Q–M)을 C++로 구현하여, 
주어진 불 대수(Boolean Function)를 자동으로 최소 SOP(Sum of Products) 형태로 변환하고 회로 비용(트랜지스터 수)까지 계산한다.

---
## 프로그램 구성 요약

본 구현은 다음을 수행한다:
- 입력으로 minterm(`m`)과 don't care(`d`)를 받아,  
- 반복적인 항 결합(reduce)으로 Prime Implicant(PI)를 도출하고,  
- Essential Prime Implicant(EPI)를 식별한 뒤,  
- 남은 항을 가장 적은 수의 PI로 덮는 탐욕적(그리디) 선택을 통해  
- 최소화된 SOP 표현과 비용(Cost)을 출력한다.

---
## 파일 구성

- 파일 구조
```bash
├─ QuineMcklusky.cpp                 # C++ 구현
├─ input_minterm.txt                 # 입력 예시
├─ result.txt                        # 결과(실행 후 생성)
└─ Implementation of Quine-McCluskey Algorithm.pdf  # 보고서
```

- 입력 파일: `input_minterm.txt`

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

- 출력 파일: `result.txt`

<선택된 PI 한 줄씩>

Cost (# of transistors): <정수>

---
## Demo Build & Run 
### Linux

```bash
# Compile
g++ QuineMcCluskey.cpp -std=c++17 -o quine

# Run (input_minterm.txt와 동일 디렉터리)
./quine

# 결과 확인
cat result.txt
```

---
## 검증 사례 (예시)

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


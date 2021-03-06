## Chapter4 문자열 최적화
## 4.1 문자열이 왜 문제인가요
- 문자열에는 구현과 관계없이 **비용이 높은 함수**가 존재한다.
- 이 함수들은 메모리를 동적으로 할당하고 표현식에서 값처럼 작동하며 내부에서 **복사**를 많이 한다.

### 4.1.1 문자열은 메모리를 동적으로 할당합니다
- 문자열은 필요한 문자열 길이보다 **더 큰 공간을 동적으로 할당**한다.
  - 동적 할당은 다른 C++ 기능보다 비용이 많다.
- 요청한 문자열 길이보다 큰 2의 거듭제곱값으로 할당을 하는식이다.
- 동적으로 할당되기에 **수동 해제 방법보다 편리**하다.
- 동적으로 할당되기에 **최적화**가 필요한 부분이 될 수 있다.

### 4.1.2 문자열은 값입니다
- 문자열은 **대입문**과 **표현식**에서 **값**처럼 작동한다.

```cpp
std::string s1, s2;
s1 = "HELLO";
s2 = s1;
s1[0] = 'T';
```
- s1은 `TELLO`, s2는 `HELLO`가 된다.
- 문자열이 값이기 때문에 문자열 표현식의 결과도 값이다.
- s1 = s2 + s3 + s4 처럼 문자열을 연결하면 s2 + s3의 결과는 새로 할당된 **임시 문자열 값**이다.
  - 이 임시 문자열에 s4를 연결하면 **또 다른 임시 문자열 값**이 된다. 이 값은 s1의 이전 값을 대체한다.
  - 메모리 관리자를 많이 호출하게 된다.

### 4.1.3 문자열을 복사를 많이 합니다
- 문자열은 값처럼 작동하므로 문자열을 수정하더라도 **다른 문자열은 수정되지 않아야** 한다.
- 하지만 문자열에는 내용을 수정하는 함수들이 있어서 문자열은 복사본을 가진 것처럼 작동해야 한다. `(COW : copy-on-write)`
- `C++11` 표준에서는 `COW` 방법이 문제가 발생할 수 있어 허용하지 않는다.
- `C++11` 표준 이후에는 **우측값 참조**와 **이동 문법** 덕분에 복사하는 비용이 많이 낮아졌다.
- 문자열이 **우측값 표현식**이라면 포인터를 복사하기 때문에 비용이 낮으며 복사본을 만들지 않아도 된다.
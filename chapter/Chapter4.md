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

## 4.2 문자열 최적화 첫 번째 시도
```cpp
std::string remove_ctrl(std::string s)
{
    std::string result;
    for (int i = 0; i < s.length; i++)
    {
        if (s[i] >= 0x20)
        {
            result = result + s[i];
        }
    }
    return result;
}
```
- 위 함수에서 `result = result + s[i]`에서 성능의 문제가 발생할 수 있다.
- 문자열 연결 연산자는 비용이 높다.
- 위 코드에서 문자열의 길이가 100이라면 **저장 공간을 만들기 위해** 메모리 관리자를 100번 호출하고 **저장 공간을 해제하기 위해** 100번 호출하게 된다.

### 4.2.1 문자열의 내용을 변경하는 연산자로 임시 문자열 제거하기
```cpp
std::string remove_ctrl_mutating(std::string s)
{
    std::string result;
    for (int i = 0; i < s.length(); ++i)
    {
        if (s[i] >= 0x20)
        {
            result += s[i];
        }
    }
    return result;
}
```
- `result += s[i];`로만 바꿔줘도 **약 13배**가 개선된다.
  - 연결 표현식의 결과를 저장하려고 임시 문자열 객체를 할당하는 코드를 제거
  - 임시 문자열 객체와 연관된 복사 및 삭제 연산 제거

### 4.2.2 저장 공간을 예약해 재할당 줄이기
```cpp
std::string remove_ctrl(std::string s) 
{
    std::string result;
    result.reserve(s.length());
    for (int i = 0; i < s.length(); ++i) 
    {
        if (s[i] >= 0x20)
        {
            result += s[i];
        }
    }
    return result;
}
```
- 위 코드에서 3,4번째줄 사이에 `result.reserve(s.length());`코드를 통해 **저장 공간을 미리 예약**한다.
- 이 작업으로 1번의 결과보다 `17%`정도의 성능향상을 할 수 있다.

### 4.2.3 문자열 인수의 복사 제거하기
```cpp
std::string remove_ctrl_ref_args(std::string const& s) 
{
    std::string result;
    result.reserve(s.length());
    for (int i = 0; i < s.length(); ++i) 
    {
        if (s[i] >= 0x20)
        {
            result += s[i];
        }
    }
    return result;
}
```
- 함수의 인자에 참조를 건다. (`std::string const& s`)
  - 이 방법은 **2번**의 방법보다 **8%**가 느려진다!
- 비주얼 스튜디오 2010은 함수 호출 시 문자열의 값을 복사하므로 **인수의 복사를 제거**하면 할당 비용을 절약할 수 있다.
  - 실제로 비용이 절약되지 않았거나 `s`를 문자열 참조로 바꿔서 절약한 비용을 다른 곳에서 소비중이어야 한다.
- 참조 변수는 포인터로 구현된다.
- 위 코드에서는 `s`가 참조라 포인터를 역참조 해야한다.
  - **역참조 하는 비용** 때문에 성능이 저하된 것이다.

### 4.2.4 반복자로 포인터 역참조 제거하기
```cpp
std::string remove_ctrl_ref_args_it(std::string const& s)
{
    std::string result;
    result.reserve(s.length());
    for (auto it = s.begin(), end = s.end(); it != end; ++it)
    {
        if (*it >= 0x20)
        {
            result += *it;
        }
    }
    return result;
}
```
- 문자열에 반복자를 사용하면 성능 저하 문제를 해결할 수 있다.
  - 반복문을 `for(auto it = s.begin(), end = s.end(); it !=end; ++it)`로 바꿔준다.
- `s.end()`값을 `end = s.end();`에서 반복문을 초기화하는 부분에 캐싱하면 2N(N은 문자열의 길이)번의 간접 참조 비용을 절약할 수 있다. 

### 4.2.5 반환된 문자열 값의 복사 제거하기
```cpp
void remove_ctrl_ref_result_it(std::string& result, std::string const& s)
{
    result.clear();
    result.reserve(s.length());
    for (auto it = s.begin(), end = s.end(); it != end; ++it)
    {
        if (*it >= 0x20)
        {
            result += *it;
        }
    }
}
```
- 호출시 문자열 변수를 가리키는 참조를 형식 인수인 `result`로 전달한다.
  - `result`가 참조하는 문자열 변수가 비어 있으면, `reserve()`를 호출해 문자를 저장하는 데 필요한 공간을 할당한다.
  - 함수를 반환할 때 호출자의 문자열 변수에 반환값을 저장하리 때문에 복사본은 필요하지 않다.
- `2%`의 성능 개선을 기대할 수 있다.

### 4.2.6 문자열 대신 문자 배열 사용하기
```cpp
void remove_ctrl_cstrings(char* dest, char const* src, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (src[i] >= 0x20)
        {
            *dest++ = src[i];
        }
    }
    *dest = 0;
}
```
- `std::string`대신 C스타일의 문자열 함수를 사용하면 성능 향상을 기대할 수 있다.
- C 스타일로 작성한 위 코드는 5번 항목의 함수보다 6배가 빠르고 처음 함수보다 170배가 빠르다.
- 함수 호출을 없애고 **캐시 지역성을 향상**했기 때문이다.

### 4.2.7 첫 번째 최적화 시도 요약
- 메모리 할당 및 관련 복사 연산을 제거하는 작업처럼 간단한 규칙을 적용해 성능 개선을 할 수 있다.
- 릴리즈 빌드에서 성능 개선이 얼마나 되었는지 더욱 뚜렷하게 나타난다.
  - [암달의 법칙](/chapter/Chapter3.md) 때문이다.
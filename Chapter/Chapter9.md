# Chapter9 검색 및 정렬 최적화
## 9.1 std::map과 std::string을 사용한 키/값 테이블
- 일반적인 키/값 테이블을 검색하고 정렬할 때의 성능을 살펴본다.
- 키타입은 `ASCII` 문자열로 `C++` 문자열 리터럴로 초기화되거나 `std::string`에 저장된다.
- 값타입은 **검색 성능**에 영향을 미치지 않는다.
  - 엄청 큰 값일 경우 캐시 성능을 저하시킬수 있다.
  - 단순한 값 타입을 사용했을 때 성능이 더 좋으므로 여기에서는 **부호가 없는 정수**를 사용한다.
- `std::map`을 사용하면 `std::string`타입을 갖는 이름을 값으로 매핑하는 테이블을 손쉽게 만들 수 있다.
```cpp
std::map<std::string, unsigned> const table;
```
- C++11 컴파일러를 사용할 경우 이니셜 라이저 문법을 사용해 초기화 할수 있다.
```cpp
std::map<std::string, unsigned> const table{
    {"alpha", 1}, {"bravo", 2},{"charlie", 3}, {"delta", 4}
};
```
- 이니셜 라이저 문법을 사용하지 않으면 각 요소를 삽입해야 한다.
```cpp
std::map<std::string, unsigned> table;
table["alpha"] = 1;
table["bravo"] = 2;
table["charlie"] = 3;
table["delta"] = 4;
```
- 간단히 값을 검색하거나 테스트 할 수 있다.
```cpp
unsigned val = table["echo"];
std::string key = "diamond";
if (table.find(key) != table.end())
{
  std::cout << "table contains " << key << std::endl;
}
```
- `std::map`으로 테이블을 만드는 코드는 C++ 표준 라이브러리가 얼마나 **강력한 추상화**를 제공하는지 보여준다.
  - 시간을 **최소한**으로 줄이면서 **합리적인 성능**을 달성할 수 있다.
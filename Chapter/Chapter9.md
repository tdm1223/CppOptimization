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

## 9.2 검색 성능 향상을 위한 툴킷
#### 테이블을 검색하는 코드를 포함한 함수가 프로그램에서 가장 많이 실행되는 함수라면?
```cpp
void HotFunction(std::string const& key)
{
    auto it = table.find(key);
    if (it == table.end())
    {
        // 테이블에 항목이 없을 때 행동
    }
    else
    {
        // 테이블에 항목이 있을 때 행동
    }
}
```
- 이보다 더 좋은 코드를 구현할수 있을까? 아래와 같이 체계적으로 진행하는 것이 좋다.
  1. 기존 구현 코드의 성능을 측정하고 비교하기 위한 **기준치**를 얻는다.
  2. 최적화할 ***추상화 코드***를 확인한다.
  3. 최적화할 코드를 **알고리즘**과 **자료구조**로 분해한다.
  4. 최적이 아닌 **알고리즘**과 **자료구조**를 변경하거나 바꾼다. 변경한 코드가 효과적인지 실험한다.

### 9.2.1 측정 기준치를 만드세요
- [챕터 3.2.2 내용](/Chapter/Chapter3.md)처럼 최적화되지 않은 코드의 성능을 측정해서 최적화한 코드를 **테스트하기 위한 측정 기준치**를 얻어야 한다.

### 9.2.2 최적화할 코드를 확인하세요
- 최적화할 코드를 확인해 여러 조각으로 분해하고 최적화 후보듣 더 쉽게 찾을수 있도록 만드는 단계이다.
- 최적화할 코드는 개발자 입장에서 판단해야 한다.
- **많이 실행되는 코드**를 통해 최적화할 코드를 찾아 볼 수 있다.
- 기본 구현 코드에서 문제 설명에 얽매이지 않기 위해 **추상적으로 문제를 작성**한다.
  - 위 예제의 경우 **키/값 테이블에서 텍스트 키로 값을 검색하는 문제**로 작성할 수 있다.

### 9.2.3 최적화할 코드를 분해하세요
- 최적화할 코드를 **알고리즘**과 **자료구조**로 분해한다.
- 최적화할 코드는 **텍스트 키가 있는 키/값 테이블에서 값을 검색**하는 코드이다.
- 기본 해결책을 구성하는 알고리즘과 자료구조는 재사용할 수 있다.
- 기존 해결책으로 사고 방법을 제한하지 않고 일반화해 알고리즘과 자료구조를 설명하는 것이 중요하다.

#### 최적화할 코드 점검
- 기본 해결책에서 테이블은 `std::map`이다.
- 기본 해결책에서 키는 `std::string`의 인스턴스이다.
- `std::map`의 템플릿은 3번째 인자로 [키를 비교하는 함수를 지정할 수 있는 매개변수](https://en.cppreference.com/w/cpp/container/map)를 제공한다.
- 많이 실행되는 함수는 `operator[]`를 사용하지 않고 `std::map::find()`함수를 호출한다.
- `std::map`은 **균형 이진 트리**로 구현되어 있으므로 각 노드를 생성해야 하는 **연결 리스트 기반 자료구조**이다.
  - 삽입 알고리즘이 있어야 하고 삽입시 비용이 많이 든다.

### 9.2.4 알고리즘과 자료구조를 변경하거나 바꾸세요
 - 기존 해결책에서 **최적이 아닌 알고리즘**을 찾는다.
  - 자료구조에서 최적화할 필요가 없거나 비용이 많이 드는 작동을 찾아 제거하거나 단순하게 만들 수 있다.
 - `std::map`은 균형 이진 트리로 구현되어 있어 검색 시간 복잡도는 `O(logn)`이다.
  - **검색시간 비용이 낮은 자료구조**로 바꿀 수 있다면 성능이 개선될 것이다.
 - `std::map`은 노드 기반 자료구조를 사용한다.
  - 생성 과정에서 **메모리 관리자를 자주 호출**해 **캐시 지역성**이 좋지 않다.
  - 최적화할 코드에서 항목은 테이블을 생성하는 과정에서만 삽입되고 테이블이 존재하는 동안에는 삭제되지 않는다.
- 키 자료구조에 필요한 기능은 **문자를 포함**하는 기능과 **두 키를 비교**하는 기능이다.
  - `std::string`은 두 가지 기능 외에 불필요한 더 많은 기능을 제공한다.

## 9.3 std::map을 사용한 검색 최적화
- 테이블의 자료구조만 남겨두고 **키를 나타내는 자료구조**와 **키를 비교하는 알고리즘**을 변경해 성능을 향상 시킬 수 있다.

### 9.3.1 std::map에 고정된 크기를 갖는 문자열 키를 사용하세요
- 테이블을 만드는 비용의 대부분은 **할당 비용**이 차지한다.
```cpp
unsigned val = table["zule"];
```
- 테이블이 `std::string` 키를 사용하는데 위처럼 C 스타일 문자열로 검색한다면 `char*` 문자열 리터럴을 `std::string`으로 변환해야 한다.
- 키의 **최대 길이가 너무 길지 않다면** 가장 긴 키를 저장할 수 있는 **char 배열이 포함된 클래스 타입**을 키로 사용하는 방법이 있다.

```cpp
std::map<char[10], unsigned> table;
```
- `C++` 배열에는 내장된 비교 연산자가 없기 때문에 배열을 직접 사용할수는 없다.
- 고정된 크기를 갖는 문자 배열 템플릿 클래스를 정의하여 사용한다.

#### 고정된 크기를 갖는 문자 배열 템플릿 클래스
```cpp
template<unsigned N = 10, typenmame T = char> struct charbuf{
  charbuf();
  charbuf(charbuf const& cb);
  charbuf(T const* p);
  charbuf& operator=(charbuf const& rhs);
  charbuf& operator=(T const* rhs);

  bool operator==(charbuf constt& that) const;
  bool operator<(charbuf const& that) const;

private:
  T data_[N];
}
```
- 문자열의 크기가 내부 버퍼의 고정된 크기보다 작고 문자열 뒤에 0인 바이트가 있는 경우에만 처리가 가능하다.
  - `std::string`처럼 안전하다는 보장은 없다.
- `std::string`을 키로 사용한 방법보다 **2배** 정도 속도가 향상되었다.

### 9.3.2 std::map에 C 스타일 문자열 키를 사용하세요
- C 스타일의 `NULL`로 끝나는 문자열을 `std::map`의 키 타입으로 사용하는 프로그램은 `char*`를 직접 사용하기 때문에 `std::string`의 인스턴스 생성/파괴 비용을 피할 수 있다.
- `char*`를 키타입으로 사용할경우 문자열 비교에서 포인터가 가리키는 문자열이 아닌 포인터 자체(주소값)를 비교한다.
  - **사용자 정의 비교 알고리즘**을 구현해야 한다.

#### 사용자 정의 비교 알고리즘
```cpp
bool less_free(char const* p1, char const* p2)
{
  return strcmp(p1, p2) < 0;
}

std::map<char const*, unsigned, bool(*)(char const*, char const*)> table(less_free);
```

#### 함수 객체를 만드는 방법
- 비교 함수를 캡슐화하는 **함수 객체**를 만드는 방법도 존재한다.
```cpp
struct less_for_c_strings
{
  bool operator()(char const* p1, char const* p2)
  {
    return strcmp(p1, p2) < 0;
  }
};

std::map<char const*, unsigned, less_for_c_strings> table;
```
- `std::string`을 사용한 코드보다 약 **3배** 빠르고 `char*`와 함수로 구현한 코드보다 거의 **2배** 빠르다.

#### C++11 버전
- `char*` 키, 비교 함수를 **람다**로 사용하였다.
```cpp
auto comp = [](char const* p1, char const* p2)
{
  return strcmp(p1, p2) < 0;
};

std::map<char const*, unsigned, decltype(comp)>table(comp);
```
- 람다 타입에는 이름이 없으며 각 람다 타입은 고유하다.
- 람다의 타입을 알기 위해 `decltype` 키워드를 사용하였다.

### 9.3.3 키가 값에 있을 때 맵의 사촌인 std::set을 사용하기
- `std::map`은 내부적으로 **키와 값**을 결합한 구조체를 선언한다.
```cpp
template <typename KeyType, typename ValueType> struct value_type {
  KeyType const first;
  ValueType second;
  // 생성자와 대입 연산자 ...
}
```
- `std::set`은 기본적으로 `std::less`로 **두 요소 전체를 비교**하는 비교 함수를 사용한다.
- `std::set`과 자체적인 키를 포함하는 사용자 정의 구조체를 사용하려면 ?
  - 사용자 정의 구조체에 대해 `std::less`나 `operator<`를 특수화하거나 디폴트가 아닌 비교 객체를 제공해야 한다.
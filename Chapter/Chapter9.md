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

## 9.4 <algorithm> 헤더를 사용한 검색 최적화
### 9.4.1 시퀀스 컨테이너의 검색을 위한 키/값 테이블
- 시퀀스 컨테이너에서 `std::map`이나 `std::set`을 사용한 구현이 아닌 키/값 테이블을 사용한 구현을 선택해야 하는 이유 ?
  - 시퀀스 컨테이너는 맵보다 **메모리를 적게 소비**하고 **설정하는 비용이 저렴**하다.
```cpp
struct kv { 
  char const* key;  // 키
  unsigned value;   // 무엇이든 될 수 있다.
}
```
- 표준 라이브러리 알고리즘의 유용한 특성중 하나는 모든 타입의 배열을 처리할 수 있다는 것이다.
- 표준 라이브러리 컨테이너 클래스는 멤버 함수 `begin()`과 `end()`를 제공하기 때문에 프로그램은 반복자를 사용해 범위 기반 검색을 할 수 있다.
- C 스타일 배열은 원시적이므로 반복자 함수를 제공하지 않아 구현해야 한다.
```cpp
// C 스타일 배열에서 크기와 시작 / 끝을 구하기
template<typename T, int N> size_t size(T(&a)[N])
{
    return N;
}
template<typename T, int N> T* begin(T(&a)[N])
{
    return &a[0];
}
template<typename T, int N> T* end(T(&a)[N])
{
    return &a[N];
}
```

### 9.4.2 이름이 명확하고 시간 비용이 O(n)인 std::find()
- `algorithm` 헤더의 템플릿 함수 `find()`
```cpp
template<class It, class T> find(It first, It last, const T& key)
```
- `find()` 알고리즘은 **선형 검색 알고리즘**이다.
- 선형 검색은 가장 일반적인 검색 유형으로 검색할 테이터가 **정렬되지 않아도 된다.**
- `find()`는 시퀀스 컨테이너에서 `key`와 똑같은 값을 갖는 **첫 번째 항목**을 가리키는 반복자를 반환한다.
```cpp
kv* result = std::find(std::begin(names), std::end(names), key);
```

### 9.4.3 값을 반환하지 않는 std::binary_search()
- 이진 검색은 **분할 정복 전략**을 사용한다.
- 표준 라이브러리 알고리즘 `binary_search()`는 **정렬된 테이블에 키가 있는지** 나타내는 `bool` 값을 반환한다.
  - 일치하는 테이블 요소를 반환하는 함수는 없다.
- 테이블에서 값을 찾는 대신 **값의 존재 여부**만 알고 싶다면 `std::binary_search()`를 사용하면 된다.

### 9.4.4 std::equal_range()를 사용한 이진 검색
- 시퀀스 컨테이너가 정렬되어 있다면 `C++` 표준 라이브러리에서 제공하는 것을 사용해 효율적인 검색 함수를 맞출 수 있다.
- 표준 라이브러리에서 제공하는 함수들은 이진 검색의 개념을 연상시키는 이름을 갖지 않는다.
```cpp
template<class ForwardIt, class T>
std::pair<ForwardIt, ForwardIt>
  equal_range(ForwardIt first, ForwardIt last, const T& value);
```
- `equal_range()`는 **정렬된 시퀀스**에서 값과 똑같은 항목들을 포함하는 **서브 시퀀스의 범위**를 구분한 반복자 쌍을 반환한다.
  - 값과 똑같은 항목이 없다면 똑같은 값을 가리키는 반복자 쌍을 반환한다.
- 반환된 반복자가 다른 값을 가리킨다면 적어도 값과 일치하는 항목이 1개는 존재한다는 것이다.
- `equal_range()`의 시간 복잡도는 `O(logN)`을 보장한다.
```cpp
auto res = std::equal_range(std::begin(names), std::end(names), key);
kv* result = (res.first == res.second) ? std::end(names) : res.first;
```
- 그러나 성능 측정 결과 선형 검색보다 결과가 좋지 않다.
  - `equal_range()`가 이진 검색 함수를 위한 최선의 선택이 아니라는 것을 알 수 있다.

### 9.4.5 std::lower_bound()를 사용한 이진 검색
- `std::lower_bound()`를 사용한 이진 검색
```cpp
kv* result = std:lower_bound(std::begin(names), std::end(names), key);
if (result != std::end(names) && key < *result.key)
{
  result = std::end(names);
}
```
- `std::lower_bound()`는 키가 `key`보다 **작지 않은 테이블의 첫 번째 항목**을 가리키는 반복자를 반환한다.
- 모든 항목이 `key`보다 작을 경우 테이블의 **끝을 가리키는 반복자**를 반환한다.
- `std::equal_range()`보다 **86%** 빠르다.
- `std::map`을 사용해 구현한 코드와 견줄만한 성능을 갖는다.
- 정적 테이블을 생성하거나 파괴하는 비용이 0이라는 장점도 있다.

### 9.4.6 직접 코딩한 이진 검색
- 표준 라이브러리 함수와 똑같은 인수를 사용해 **이진 검색을 직접 구현**할 수 있다.
- 표준 라이브러리 알고리즘은 모두 **하나의 비교 함수 operator<()**를 사용한다.
  - 최소한의 인터페이스만 제공하면 된다.
```cpp
kv* find_binary_lessthan(kv* start, kv* end, char const* key)
{
    kv* stop = end;
    while (start < stop)
    {
        auto mid = start + (stop - start) / 2;
        if (*mid < key)
        {
            start = mid + 1;
        }
        else
        {
            stop = mid;
        }
    }
    return (Start == end || key < *start) ? end : start;
}
```
- `std::lower_bound()`를 사용한 이진 검색과 거의 비슷한 성능을 갖는다.

### 9.4.7 strcmp()를 사용해 직접 코딩한 이진 검색
```cpp
kv* find_binary_3(kv* start, kv* end, char const* key)
{
    auto stop = end;
    while (start < stop)
    {
        auto mid = start + (stop - start) / 2;
        auto rc = strcmp(mid->key, key);
        if (rc > 0)
        {
            stop = mid;
        }
        else if (rc < 0)
        {
            start = mid + 1;
        }
        else
        {
            return mid;
        }
    }
    return mid;
}
```
- `operator<()`를 `strcmp()`관점에서 정의하여 최적화 할 수 있다.
- 표준 라이브러리에서 사용하는 이진 검색보다 **26%** 빠르다.

## 9.5 해시 키/값 테이블 검색 최적화
- `C++`은 `std::hash`라고 하는 **표준 해시 함수 객체**를 정의한다.
- `std::hash`는 정수, 부동 소수점 데이터, 포인터, `std::string`타입에 **특수화된 템플릿**이다.
- 특수화되지 않은 `std::hash`는 포인터로도 사용한다.
  - 해시된 타입을 `size_t`로 변환한 뒤 값의 비트를 무작위로 추출한다.
- 해시 테이블 구현시 고려해야 할 점은 효율적인 해시 함수를 찾는 것이다.
  - 동일한 해시 값을 가질때 **충돌**이 일어나게 되는데 **충돌**을 해결하는 방법을 제시해야 한다.
- 해시 함수가 **특정 색인값을 절대 생성하지 않는 경우**가 발생할 수 있다.
  - 해당 색인 값은 해시 테이블에서 **절대 사용되지 않는 상태**로 남아있게 된다.
  - 해시 테이블의 크기가 같은 항목을 저장하는 정렬된 배열보다 커지게 된다.

### 9.5.1 std::unordered_map으로 해싱하기
#### std::unordered_map을 사용한 해시 테이블 초기화 예제
```cpp
std::unordered_map<std::string, unsigned> table;
// 해시 테이블 초기화
for (auto it = names; it != names + namesize; ++it)
{
  table[it->key] = it->value;
}
// 검색
auto it = table.find(key);
```
- `std::unordered_map`에서 사용되는 **디폴트 해시 함수**는 템플릿 함수 객체 `std::hash`이다.
- `std::unordered_map`에 **std::string 키**를 사용했을때 성능은 **문자열 키**를 사용하는 `std::map`보다 **56%** 빠르다.

### 9.5.2 고정된 크기를 갖는 문자 배열 키로 해싱하기
```cpp
template<unsigned N = 10, typenmame T = char> struct charbuf{
  charbuf();
  charbuf(charbuf const& cb);
  charbuf(T const* p);
  charbuf& operator=(charbuf const& rhs);
  charbuf& operator=(T const* rhs);

  operator size_t() const; // 해시 함수

  bool operator==(charbuf constt& that) const;
  bool operator<(charbuf const& that) const;

private:
  T data_[N];
}

std::unordered_map<charbuf<>, unsigned> table;
```
- 9.3에서 사용한 고정된 문자 배열 템플릿 클래스인 `charbuf`에 해시 함수를 추가하였다.
- 추가한 해시 함수는 직관적인 이름이 아니고 명확하지도 않다.
- 성능은 `std::string`을 사용한 해시테이블이나 맵보다 오래 걸린다.

### 9.5.3 NULL로 끝나는 문자열 키로 해싱하기
- 해시 테이블을 `C++`문자열 리터럴처럼 수명이 길고 `NULL`로 끝나는 문자열로 초기화 할 수 있다면 ?
  - **문자열을 가리키는 포인터**로 해시 기반의 키/값 테이블을 생성할 수 있다.
- `std::unordered_map`의 키 타입으로 `char*`를 사용하면 성능을 향상시킬 수 있다.
  - `key`가 포인터일 때 `Hash`는 문제 없이 정의되고 오류 없이 컴파일 된다.
- `std::hash`가 **포인터가 가리키는 문자열의 해시 값**이 아닌 **포인터 자체의 해시 값**을 생성하기 때문에 올바른 프로그램이 아니다.
- 사용자 입력에서 가져온 문자열을 사용해 테스트하면 **테스트할 문자열을 가리키는 포인터**와 **테이블 초기화에 사용한 키를 가리키는 포인터**가 다르기 때문에 문자열이 테이블에 없게 된다.
- `std::unordered_map`템플릿의 세 번째 인자에 기본값 대신 **사용자 정의 해시 함수**를 사용하면 된다.
- `std::unordered_map`템플릿의 네 번째 인자인 **KeyEqual**도 수정해야 한다.
  - 인자의 기본값은 두 피연산자에 `operator==`을 적용하는 함수 객체인 `std::equal_to`이다.
  - `operator==`는 포인터에 대해 정의되어 있지만 **포인터가 가리키는 문자열**을 비교하는게 아니라 **포인터가 가리키는 주소**만 비교한다.
  - `KeyEqual` 템플릿 매개변수에 **사용자 정의 함수 객체**를 사용하면 해결된다.

```cpp
// 사용자 정의 해시 함수
struct hash_c_string
{
    void hash_combine(size_t& seed, const char& v) const 
    {
        seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    std::size_t operator()(char const* p) const 
    {
        size_t hash = 0;
        for (; *p; ++p)
        {
            hash_combine(hash, *p);
        }
        return hash;
    }
};

// 비교에 사용하는 사용자 정의 함수 객체
struct comp_c_string
{
    bool operator()(char const* p1, char const* p2) const 
    {
        return strcmp(p1, p2) == 0;
    }
};
```
- `std::string`을 기반으로 하는 해시 테이블보다 **73%** 빠르다.
- `std::map`과 `char*` 키를 기반으로 하는 최적의 구현 코드보다 **9%** 빠르다.

### 9.5.4 사용자 정의 해시 테이블로 해싱하기
- 주어진 키 집합에 대해 **충돌이 없는 테이블**을 만드는 해시 함수를 **완벽한 해시 함수**라고 한다.
- 사용하지 않는 공간이 없는 테이블을 만드는 해시 함수를 **최소 해시 함수**라고 한다.
- 이상적인 해시 함수는 충돌이 없고 빈 공간이 없는 테이블을 만드는 완벽한 최소 해시 함수이다.
- 완벽한 최소 해시 함수는 만들기 어렵다.
- 해시 함수로 사용할 수 있는 예로는 첫번째 문자, 문자의 합, 키의 길이 등이 있다.

## 9.6 스테파노프의 추상화 패널티
- **이진 검색**은 **선형 검색**보다 빠르며 **해싱**은 **이진 검색**보다 빠르다.
- `C++` 표준 알고리즘과 직접 코딩한 알고리즘 간의 성능 차이를 스테파노프의 추상화 패널티라고 한다.
- 표준 라이브러리의 매우 강력하고 범용적인 기능을 사용하려면 비용이 든다.
  - 우수한 성능을 갖는 표준 라이브러리 알고리즘이 있더라도 **직접 코딩한 최적의 알고리즘**과 경쟁할 수는 없다.
- 스테파노프의 추상화 패널티는 사용자 정의로 직접 코딩한 해결책과 대조적으로 보편적인 해결책을 제공하는 필연적인 비용이다.
  - `C++` 표준 라이브러리의 알고리즘처럼 매우 생산적인 도구를 사용하기 위해 내는 통행료 같은 것이다.

## 9.7 C++ 표준 라이브러리로 정렬 최적화
- 시퀀스 컨테이너에서 **분할 정복 알고리즘**을 사용해 효율적으로 검색하기 위해서는 **정렬된 상태**여야 한다.
- 정렬을 위한 두 알고리즘 `std::sort()`와 `std::stable_sort()`를 제공한다.
  
#### std::sort
- `std::sort`는 **퀵 정렬**을 변형해서 구현하였다.
- `C++03`에서는 `std::sort`의 평균의 경우 성능이 `O(nlogn)`이어야 한다.
  - 구현 코드가 `C++03` 표준을 준수한다면 퀵 정렬을 사용해 `std::sort`를 구현한다.
  - 최악의 경우 성능 O(n<sup>2</sup>)을 줄이기 위해 **중앙값을 고르는 트릭**을 사용한다.
- `C++11`에서는 최악의 경우 성능이 O(nlogn)이어야 한다.
  - 구현 코드가 `C++11` 표준을 준수한다면 보통 **팀 정렬**이나 **인트로 정렬**과 같은 하이브리드 정렬 알고리즘을 사용한다.

#### std::stable_sort
- 안정 정렬은 프로그램이 여러 기준별로 다양한 레코드들을 정렬할 수 있고 여러 기준을 순서대로 적용할 수 있다는 장점이 있다.
- `std::stable_sort`는 **병합 정렬**을 변형한 코드를 사용한다.
- 할당할 수 있는 추가 메모리가 충분하다면 `O(nlogn)` 시간에 그렇지 않다면 O(n(logn)<sup>2</sup>)시간에 실행된다.
- 재귀 깊이가 너무 깊지 않다면 **병합 정렬**을 사용하고 그렇지 않으면 **힙 정렬**을 사용하는 방법으로 구현한다.

#### C++ algorithm헤더에 있는 다양한 정렬 알고리즘
- `std::heap_sort`는 [힙 속성을 갖는 범위](https://en.wikipedia.org/wiki/Heap_(data_structure))를 정렬된 범위로 변환한다.
  - `heap_sort`는 **안정 정렬**이 아니다.
- `std::partition`은 **퀵 정렬**의 기본 작동을 수행한다.
- `std::merge`는 **병합 정렬**의 기본 작동을 수행한다.
- 시퀀스 컨테이너에 있는 `insert` 멤버 함수는 **삽입 정렬**의 기본 작동을 수행한다.
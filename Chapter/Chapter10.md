# Chapter10 자료구조 최적화
## 10.1 표준 라이브러리 컨테이너 알아보기
### 10.1.1 시퀀스 컨테이너
- `std::string`, `std::vector`, `std::deque`, `std::list`, `std::forward_list`
- 항목을 **삽입한 순서대로 저장**한다.
- 각 컨테이너에는 **앞과 뒤**가 있다.
- `std::forward_list`를 제외한 모든 시퀀스 컨테이너에는 상수 시간에 항목을 뒤에 삽입할 수 있는 멤버 함수(push_back)가 있다.
- 항목을 앞에 효율적으로 삽입(`push_front`)할 수 있는 시퀀스 컨테이너는 `std::deque`, `std::list`, `std::forward_list`이다
- `std::vector`, `std::deque`, `std::string`의 내부는 **배열과 같은 구조**로 이루어져 있다.
  - 컨테이너에서 항목을 삽입하면 삽입할 위치 뒤에 있는 모든 항목을 한 칸씩 밀어내야 한다.(최악의 경우 `O(n)`)
  - 항목 삽입시 내부 배열이 재할당 될 수 있으며 모든 반복자와 포인터가 무효화 된다.
- `std::list`, `std::forward_list`의 인스턴스는 반복자를 유효한 상태로 유지하면서 이어 붙이고 병합할 수 있다.
  - 중간에 항목을 삽입하는 비용은 **상수 시간**이다.

### 10.1.2 연관 컨테이너
- 항목을 삽입한 순서대로 저장하지 않고 항목의 **순서 특성에 기반**해 저장한다.
- 모든 연관 컨테이너는 항목에 **효율적으로 접근하는 방법**을 제공한다.
  - 선형시간 이하의 복잡도를 갖는다.
- 맵과 셋은 서로 다른 인터페이스를 제공한다.
  - 맵은 별도로 정의된 **키와 값**을 저장한다.
  - 셋은 순서가 지정된 **고유한 값**을 저장하며 값의 존재 여부를 효율적으로 매핑한다.
  - 멀티맵과 멀티셋은 같은 항목을 여러 번 삽입할 수 있다.
- 항목을 삽입하거나 삭제하는 시간은 `O(logN)`이다.
- 맵, 셋, 멀티맵, 멀티셋은 **균형 이진 트리**를 퍼사드 패턴으로 구현한다.
- `C++11` 이후에는 순서가 지정되지 않은 연관 컨테이너 4종이 추가되었다.
  - `std::unordered_map`, `std::unordered_multimap`, `std::unordered_set`, `std::unordered_multiset`이다.
- 순서가 지정되지 않은 연관 컨테이너는 키나 항목이 서로 동일한 관계인지 확인하는 함수만 정의하면 된다.
- 순서가 지정되지 않은 연관 컨테이너는 해시 테이블로 구현한다.
  - 항목을 **삽입하거나 삭제하는 시간은 상수 시간**이며 최악의 경우 `O(n)`이 걸린다.

## 10.2 std::vector와 std::string
#### std::vector와 std::string의 특징
- 시퀀스 컨테이너
- 삽입 시간 : 뒤에서 삽입할 경우 `O(1)`, 그외 경우 `O(n)`
- 색인 시간 : 위치로 색인할 경우 `O(1)`
- 정렬 시간 : `O(nlogn)`
- 검색 시간 : 정렬되어 있다면 `O(logn)`, 그외 경우 `O(n)`
- 내부 배열을 재할당하면 반복자와 참조가 무효화 된다.
- 반복자는 양방향으로 항목을 방문한다.
- 크기와 관계없이 할당된 용량을 합리적으로 제어한다.

### 10.2.1 재할당의 성능 결과
- `std::vector`에는 요소의 개수를 나타내는 크기(`size`)와 요소를 저장하는 내부 버퍼의 크기를 나타내는 용량(`capacity`)이 있다.
- 크기와 용량이 같을때 요소를 삽입하면 **확장 작업**을 수행한다.
  - 내부 버퍼를 재할당 하고 벡터의 요소들을 새 저장 공간으로 복사한 뒤 기존 버퍼의 모든 반복자와 참조를 무효화한다.
- `std::vector`를 효율적으로 사용하는 방법은 `reserve`를 호출해 사용하기 전에 **용량을 예약**하는 것이다.
  - 용량을 예약하면 불필요한 재할당과 복사 작업을 하지 않아도 된다.
- `clear`는 컨테이너의 **크기를 0으로 설정**한다.
  - 내부 버퍼의 용량을 줄이기 위해 재할당한다고 보장하지 않는다.
- 벡터의 메모리를 해제하려면 아래와 같은 트릭을 사용한다.
```cpp
std::vector<Foo> x;     // 빅 벡터 생성
vector<Foo>().swap(x);  // 빈 벡터의 내용과 벡터 x의 내용을 바꿈
```
- 빈 벡터를 임시로 생성한 뒤 빈 벡터의 내용과 벡터 x의 내용을 바꾼다.
  - 임시로 생성했던 벡터를 삭제해 x에 있던 모든 메모리를 회수한다.

### 10.2.2 std::vector에서 삽입/삭제하기
#### 대입
- 벡터에 데이터를 삽입하는 가장 빠른 방법은 벡터를 **대입**하는 것이다.
- 벡터의 크기를 알고 있으므로 대입할 벡터의 내부 버퍼를 만들기 위해 메모리 관리자를 한 번만 호출하면 된다.
```cpp
std::vector<int> test_container, random_vector;
test_container = random_vector;
```

#### insert
- 데이터가 다른 컨테이너에 있다면 `std::vector::insert()`를 사용해 벡터에 복사할 수 있다.
```cpp
std::vector<int> test_container, random_vector;
test_container.insert(test_container.end(), random_vector.begin(), random_vector.end());
```

#### push_back
- 새로운 항목을 벡터의 끝에 효율적으로 삽입할 수 있다.
```cpp
std::vector<int> test_container, random_vector;

// 반복자를 사용하는 코드
for (auto it = random_vector.begin(); it != random_vector.end(); ++it)
{
  test_container.push_back(*it);
}

// 멤버 함수 std::vector::at()을 사용한 코드
for (unsigned i = 0; i < random_vector.size(); ++i)
{
  test_container.push_back(random_vector.at(i)); 
}

// 첨자를 사용하는 코드
for (unsigned i = 0; i < random_vector.size(); ++i)
{
  test_container.push_back(random_vector[i]); 
}
```
- 벡터에 항목을 하나씩 삽입하기 때문에 속도가 느리다.
- 복사본 전체를 저장할 수 있을 만큼 큰 버퍼를 미리 할당해 더 효율적인 반복문을 만들 수 있다.
```cpp
std::vector<int> test_container, random_vector;
test_container.reserve(random_vector.size());
for (auto it = random_vector.begin(); it!= random_vector.end(); ++it)
{
  test_container.push_back(*it);
}
```

#### 맨 앞에 삽입
- 맨 앞에 삽입하면 새 항목을 위한 공간을 만들기 위해 **벡터의 모든 항목을 복사**해야 하므로 비효율 적이다.
- 맨앞 삽입의 시간 비용은 `O(n)`이다.
```cpp
std::vector<int> test_container, random_vector;
for (auto it = random_vector.begin(); it != random_vector.end(); ++it)
{
  test_container.insert(test_container.begin(), *it);
}
```

### 10.2.3 std::vector에서 코드 반복 실행하기
- 벡터에 대하여 코드를 반복 실행하는 방법은 세가지가 존재한다.
```cpp
std::vector<int> test_container;
unsigned sum = 0;
// 반복자
for (auto it = test_container.begin(); it != test_container.end(); ++it)
{
  sum += it->value;
}

// 멤버 함수 at
for (unsigned i = 0; i < test_container.size(); ++i)
{
  sum += test_container.at(i).value;
}

// 첨자
for (unsigned i = 0; i < test_container.size(); ++i)
{
  sum += test_container[i].value;
}
```

### 10.2.4 std::vector 정렬하기
- `std::sort()`와 `std::stable_sort()`를 통해 벡터를 정렬할 수 있다.
- 두 알고리즘의 실행 시간은 `O(nlogn)`이다.
  - 데이터가 정렬되어 있으면 실행속도가 더 빠르다.
```cpp
std::vector<int> v;
std::sort(v.begin(), v.end());
```

### 10.2.5 std::vector에서 검색하기
- `sorted_container`에서 `random_vector`에 있는 모든 키를 검색하는 코드이다.
```cpp
std::vector<int> sorted_container, random_vector;
for (auto it = random_vector.begin(); it != random_vector.end(); ++it)
{
  auto kp = std::lower_bound(sorted_container.begin(), sorted_container.end(), *it);
  if (kp != sorted_container.end() && *it < *kp)
  {
    kp = sorted_container.end();
  }
}
```

## 10.3 std::deque
- 시퀀스 컨테이너
- 삽입 시간 : 뒤에서 삽입할 경우 `O(1)`, 그외 경우 `O(n)`
- 색인 시간 : 위치로 색인할 경우 `O(1)`
- 정렬 시간 : `O(nlogn)`
- 검색 시간 : 정렬되어 있다면 `O(logn)`, 그외 경우 `O(n)`
- 내부 배열을 재할당하면 반복자와 참조가 무효화 된다.
- 반복자는 양방향으로 항목을 방문한다.
- `FIFO` 큐를 생성하는 특수화된 컨테이너이다.
- 여러 배열을 저장하는 배열로 구현한다.
  - 덱에 저장된 항목을 얻으려면 간접 참조를 두 번해야 한다.
  - 캐시 지역성이 감소하고 메모리 관리자를 더 자주 호출하게 된다.

### 10.3.1 std::deque에서 삽입/삭제하기
- 벡터와 동일한 삽입 인터페이스에 추가로 앞에 맨 앞에 요소를 삽입하는 멤버 함수 `push_front()`를 제공한다.
```cpp
std::deque<int> test_container;
std::vector<int> random_vector;

// 하나의 덱을 다른 덱으로 대입하는 코드
test_container = random_vector;

// 반복자를 사용해 덱에 삽입하는 코드
test_container.insert(test_container.end(), random_vector.begin(), random_vector.end());

// push_back을 사용해 벡터에서 덱으로 항목을 복사하는 세 가지 코드
// 반복자를 사용하는 코드
for (auto it = random_vector.begin(); it != random_vector.end(); ++it)
{
  test_container.push_back(*it);
}

// 멤버 함수 std::vector::at()을 사용한 코드
for (unsigned i = 0; i < random_vector.size(); ++i)
{
  test_container.push_back(random_vector.at(i)); 
}

// 첨자를 사용하는 코드
for (unsigned i = 0; i < random_vector.size(); ++i)
{
  test_container.push_back(random_vector[i]); 
}
```
#### 항목의 수가 같을 경우
- 벡터가 덱보다 대입하는 속도가 13배 빠르다.
- 벡터가 덱보다 삭제하는 속도가 22배 빠르다.
- 반복자 버전에서 삽입하는 속도는 벡터가 덱보다 9배, `push_back()`의 속도는 2배, 뒤쪽에서 삽입하는 속도는 3배 빠르다.

### 10.3.2 std::deque를 사용하는 반복문
- **반복자 기반 버전**은 덱이 벡터보다 빠르고 **첨자 기반 버전**은 덱이 벡터보다 느리다.
- 덱에서 속도가 가장 빠른 탐색 방법은 벡터에서 속도가 가장 빠른 탐색 방법보다 비용이 2배 많이 든다.

### 10.3.3 std::deque 정렬하기
- `std::sort()`의 경우 덱이 벡터보다 약 33% 느리다.
- `std::stable_sort()`의 경우 덱이 벡터보다 약 10% 느리다.

### 10.3.4 std::deque에서 검색하기
- 항목이 정렬되어 있는 덱에서 키를 검색하는데 비용은 덱이 벡터보다 20% 더 많이 든다.

## 10.4 std::list
- 시퀀스 컨테이너
- 삽입 시간 : 어느 위치에서나 `O(1)`
- 정렬 시간 : `O(nlogn)`
- 검색 시간 : `O(n)`
- 항목을 삭제하는 경우를 제외하고 반복자와 참조가 절대 무효화되지 않는다.
- 반복자는 양방향으로 항목을 방문한다.
- 리스트를 효율적으로 검색하는 방법은 없다.
- 리스트 항목을 복사하지 않고 `O(1)` 시간에 특정 범위의 항목을 추출하거나 병합할 수 있다.
- 리스트의 각 항목에 할당된 저장 공간의 크기는 모두 같다.
  - 메모리 관리자가 메모리를 효율적으로 관리할 수 있다.
  - 메모리 단편화가 생길 가능성이 줄어든다.

### 10.4.1 std::list에서 삽입/삭제하기
- `insert()`, `push_back()`, `push_front()`를 사용해 하나의 리스트를 다른 리스트에 복사하는 알고리즘은 벡터와 덱과 같다.
  
### 10.4.2 std::list에서 반복하기
- 리스트는 첨자 연산자가 없다.
- 리스트를 탐색하는 유일한 방법은 **반복자**를 사용하는 것이다.
- 벡터보다 약 39% 느린 성능을 보인다.

### 10.4.3 std::list 정렬하기
- 리스트의 반복자는 양방향 반복자인데 벡터의 임의 접근 반복자보다는 강력하지 않다.
- 두 양방향 반복자 사이의 거리나 항목의 개수를 찾는 비용이 `O(n)`이다.
- 리스트에서 `std::sort()`는 O(n<sup>2</sup>)의 성능을 갖는다.
- 리스트에 내장된 `sort()`의 경우 `O(nlogn)`의 성능을 보인다.
- 벡터를 정렬했을 때보다 25% 더 오래 걸린다.

### 10.4.4 std::list에서 검색하기
- 리스트는 양방향 반복자만 제공하므로 리스트에서 **이진 검색 알고리즘**의 수행 시간은 `O(n)`이다.
- `std::find()`로 검색한 시간은 `O(n)`이다.
- 리스트는 연관 컨테이너를 대체할 수 있는 후보 중에서 성능이 그다지 좋지 않다.

## 10.5 std::forward_list
- 시퀀스 컨테이너
- 삽입 시간 : 어느 위치에서나 `O(1)`
- 정렬 시간 : `O(nlogn)`
- 검색 시간 : `O(n)`
- 항목을 삭제하는 경우를 제외하고 반복자와 참조가 절대 무효화되지 않는다.
- 반복자는 앞에서 뒤로 항목을 방문한다.
- 포워드 리스트의 각 항목은 필요할 때 따로 할당된다.
  - 포워드 리스트에서 사용하지 않는 용량은 없다.
- 포워드 리스트의 각 항목에 할당된 저장 공간의 크기는 모두 같다.

### 10.5.1 std::forward_list에서 삽입/삭제하기
- 어떤 위치의 앞을 가리키는 반복자가 있을 경우 임의의 위치에 항목을 삽입하는 시간이 상수 시간이다.
- `push_front()` 함수를 통해 삽입하는 시간은 리스트일때 걸린 시간과 거의 동일하다.

### 10.5.2 std::forward_list에서 반복하기
- 포워드 리스트에는 첨자 연산자가 없다.
- 포워드 리스트를 탐색하는 유일한 방법은 반복자를 사용하는 방법이다.
- 벡터보다 약 45% 느린 성능을 보인다.

### 10.5.3 std::forward_list 정렬하기
- 리스트와 마찬가지로 포워드 리스트에도 `O(nlogn)`의 성능을 갖는 정렬이 내장되어 있다.
- 리스트에 내장된 정렬 함수의 성능과 비슷하다.

### 10.5.4 std::forward_list에서 검색하기
- 포워드 리스트는 순방향 반복자만 제공하므로 리스트에서 **이진 검색 알고리즘**의 수행 시간은 `O(n)`이다.
- `std::find()`로 검색한 시간은 `O(n)`이다.
- 포워드 리스트는 연관 컨테이너를 대체할 수 있는 후보 중에서 성능이 그다지 좋지 않다.

## 10.6 std::map과 std::multimap
- 순서가 지정된 연관 컨테이너
- 삽입 시간 : `O(logn)`
- 색인 시간 : `O(logn)` (키를 사용할 경우)
- 항목을 삭제하는 경우를 제외하고 반복자와 참조가 절대 무효화되지 않는다.
- 반복자는 항목을 정렬한 순서나 역순으로 정렬한 순서로 생성한다.
- 내부적으로 **균형 이진 트리**로 구현되어 있으며 반복자 기반의 탐색을 용이하기 위해 별도의 **링크**가 있다.
  - 트리를 사용해 구현하지만 트리는 아니다.
  - 너비 우선 탐색(`BFS`)과 같은 트리에서 할 수 있는 다른 일을 수행할 방법이 없다.
- 맵의 각 항목은 필요할 때 할당된다.
- 맵의 각 항목에 할당된 저장 공간의 크기는 모두 같다.
  - 메모리 관리자가 메모리를 효율적으로 관리할 수 있다.
  - 메모리 단편화가 생길 가능성이 줄어든다.

### 10.6.1 std::map에서 삽입/삭제하기
- 보통 맵에 삽입하는 시간은 `O(logn)`이다.
  - 삽입점을 찾기 위해 맵의 내부 트리를 탐색해야 하기 때문이다.
- 맵은 반복자를 추가 인수로 사용하는 `insert()`함수를 제공한다.
  - 이 반복자가 제공하는 힌트가 최적이라면 삽입 시간은 `O(1)`이 된다.

#### 확인 및 갱신 코드 최적화 하기
- 맵 안에 키가 있는지 확인한 다음 결과에 따라 어떤 작동을 수행하는 코드를 작성하는 경우가 있다.
- 검색된 키에 해당하는 값을 삽입하거나 갱신하는 작동이 있다면 성능을 최적화 할 수 있다.

#### 맵에 저장된 항목이 존재하는지를 확인하고 싶다면 ?
- 반복자와 `bool`값으로 된 쌍을 반환하는 `insert()`를 사용한다.
```cpp
std::pair<value_t, bool> result = table.insert(key, value);
if (result.second)
{
  // 키를 찾은 경우
}
else
{
  // 키를 찾지 못한 경우
}
```
- `bool`값은 항목을 삽입한 경우 `true` 찾은 경우 `false`가 된다.
- 프로그램이 항목의 존재 여부를 알아내기 전에 초기화하는 방법을 알고 있거나, 값을 갱신하는 비용이 크지 않을때 좋은 방법이다.

#### upper_bound(), lower_bound()를 호출해 키나 삽입점을 찾는 방법
- `lower_bound()`는 맵에서 검색 키보다 작지 않은 첫 번째 항목 또는 `end()`를 가리키는 반복자를 반환한다.
- 삽입해야 하는 경우에는 삽입 힌트가 되며 존재하는 항목을 갱신해야 하는 경우에는 키를 가리킨다.
```cpp
iterator it = table.lower_bound(key);
if (it == table.end() || key < it->first)
{
  // 키를 찾지 못한 경우
  table.insert(it, key, value);
}
else
{
  // 키를 찾은 경우
  it->second = value;
}
```

### 10.6.2 std::map를 사용한 반복문
- 맵에 저장된 항목을 반복하는 경우 벡터를 반복했을 때보다 약 10배 더 오래걸린다.

### 10.6.3 std::map 정렬하기
- 맵은 내부 구조가 기본적으로 정렬되어 있다.
- 맵을 반복하면 사용 중인 키와 검색 조건에 따라 순서대로 항목을 생성한다.
- 다른 검색 조건을 사용해 맵을 재정렬하려면 모든 요소를 다른 맵으로 복사해야 한다.

### 10.6.4 std::map에서 검색하기
- 맵에 **저장된 항목을 모두 검색**하는 경우 `std::lower_bound()`를 사용해 정렬된 벡터에서 항목을 검색하는 경우보다 50%느린 성능을 보여준다.
- 테이블에 다수의 항목을 한꺼번에 생성하고 검색하는 일이 잦다면 벡터 기반으로 구현하는 게 더 빠르다.
- 테이블에 항목을 자주 삽입하거나 삭제해서 변하는 경우가 잦다면 벡터 기반의 테이블을 정렬하는 비용이 검색에서 절약한 시간을 상쇄한다.
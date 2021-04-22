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
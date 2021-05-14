# Chapter13 메모리 관리 최적화
- 메모리 관리자는 동적 변수의 메모리 할당을 감독하는 `C++` 런타임 시스템의 함수 및 자료구조 집합이다.

## 13.1 C++ 메모리 관리 API
### 13.1.1 동적 변수의 생명 주기
- 동적 변수의 생명 주기는 5(할당, 배치, 사용, 파괴, 해제)단계로 나눌 수 있다.
- `new` 표현식은 **할당 단계**와 **배치 단계**를 수행한다.
- `delete` 표현식은 **파괴 단계**와 **해제 단계**를 수행한다.

#### 할당
- 프로그램은 메모리 관리자에게 연속적인 메모리 영역을 가리키는 포인터를 반환하도록 요청한다.
- 포인터는 적어도 지정된 수만큼의 타입이 없는 메모리 바이트를 포함하고 있어야 한다.
- 메모리를 사용할 수 없는 경우 할당을 실패할 수 있다.
- `malloc()`, `operator new()`의 다양한 오버로드가 할당 단계를 관리한다.

#### 배치
- 프로그램은 할당된 메모리에 값을 저장해 **동적 변수의 초기값**을 설정한다.
- 변수가 **클래스 인스턴스**라면 클래스의 **생성자**중 하나가 호출된다.
- 변수가 **타입**이라면 **선택적**으로 초기화 된다.
- 생성자가 예외를 던질 경우 메모리 관리자에게 할당된 저장 공간을 반환해야 하므로 배치를 실패할 수 있다.
- `new` 표현식이 배치 단계에 포함된다.

#### 사용
- 프로그램은 동적 변수의 값을 읽고 멤버 함수를 호출하며 값을 쓴다.

#### 파괴
- 동적 변수가 갖는 시스템 자원을 반환하고 정리를 완료한다.
- 변수가 클래스 인스턴스라면 프로그램은 클래스의 소멸자를 호출해 동적 변수에 대한 최종 작동을 수행한다.
- 소멸자가 처리되지 않은 예외를 던질 경우 파괴를 실패할 수 있다.
- 소멸자를 명시적으로 호출하면 저장 공간을 해제하지 않고 변수를 파괴할 수 있다.
- `delete` 표현식이 피괴 단계를 관리한다.

#### 해제
- 프로그램은 이전에 파괴된 동적 변수에 속하는 저장 공간을 메모리 관리자에게 반환한다.
-` free()`, `operator delete()`의 다양한 오버로드가 해제 단계를 수행한다.

### 13.1.2 메모리를 할당하고 해제하는 메모리 관리 함수
- `C++`은 `C`의 `malloc()`과 `free()`를 대신하는 메모리 관리 함수를 제공한다.
- 이 함수는 `new` 표현식의 작동을 다채롭게 제공한다.

#### operator new()는 할당을 구현합니다
- `new` 표현식은 `operator new()` 또는 `operator new[]()`의 여러 버전 중 하나를 호출해 동적 변수를 위한 메모리를 얻는다.
- `C++`은 두 종류의 연산자를 위한 **기본 구현을 제공**한다.
- 묵시적으로 선언하므로 `<new>`헤더를 포함하지 않고도 프로그램에서 호출할 수 있다.
- 프로그램에서 기본 구현을 재정의 할 수도 있다.
- `C++`은 `operator new()`의 여러 오버로드를 정의한다.

#### void* ::operator new(size_t)
- 동적으로 할당된 변수에 메모리를 할당한다.
- 할당할 최소 바이트 수를 지정하는 인수를 받는다.
- 메모리가 부족할 경우 `std::bad_alloc` 예외를 던진다.
- `operator new()` 오버로드의 표준 라이브러리 구현은 모두 이 오버로드를 호출한다.
- 표준 라이브러리는 보통 `malloc()`을 호출하는 방법으로 구현한다.

#### void* ::operator new[](size_t)
- 이 오버로드를 호출해 배열을 할당한다.
- 표준 라이브러리 구현은 `::operator new(size_t)`를 호출한다.

#### void* ::operator new(size_t, const std::nothrow_tag&)
- `operator new()`의 **예외를 던지지 않는 오버로드**를 호출한다.
- 메모리가 부족할 경우 `std::bac_alloc` 예외 대신 `nullptr`를 반환한다.
- 표준 라이브러리 구현은 `operator new(size_t)`를 호출하고 던지는 예외를 모두 잡는다.

#### void* ::operator new[](size_t, const std::nothrow_tag&)
- 예외를 던지지 않는 `operator new()` 오버로드의 **배열 버전**이다.
- `new` 표현식을 첫 번째 인수가 `size_t` 타입인 임의의 시그니처와 함께 `operator new()`를 호출할 수 있다.
- 메모리 지정 `operator new()`라고 부른다.
- `new` 표현식은 메모리 지정 매개변수의 인수 타입과 사용 가능한 `operator new()` 함수 시그니처를 똑같이 만들어 어떤 함수를 사용할지 결정한다.

#### void* ::operator new(size_t, void*)
- 변수를 위한 메모리 지정 `operator new()`이다.
- 메모리를 가리키는 포인터를 두 번째 인수로 받고 해당 포인터를 반환한다.

#### void* ::operator new[](size_t, void*)
- 메모리 지정 `operator new()`의 **배열 버전**이다.
- 메모리를 가리키는 포인터를 두 번째 인수로 받고 해당 포인터를 반환한다.
- 메모리 지정 `operator new()`의 두 오버로드를 메모리 `지정 new 표현식 new(p) type`이라고 한다.
- `p`는 유효한 저장 공간을 가리키는 포인터이다.
- `C++` 코드는 개발자 코드로 두 오버로드를 개체할 수 없다고 규정한다.
- 오버로드를 대체했는데 포인터 인수를 반환하는 작동이 아니면 표준 라이브러리 상당 부분이 정상적으로 작동하지 않을 것이다.

#### operator delete()는 할당된 메모리를 해제합니다
- `delete` 표현식은 `operator delete()` 또는 `operator delete[]()` 를 호출해 동적 변수에 할당된 메모리를 런타임 시스템에 반환한다.
- `new` 연산자와 `delete` 연산자는 메모리 할당과 해제를 위해 함께 사용된다.
- 특별한 메모리 풀에서 메모리를 할당하거나 특별한 방법으로 메모리를 할당하는 `operator new()`를 정의한다면 동일한 범위에서 `operator new()`와 일치하는 `operator delete()`를 정의해 메모리를 할당한 풀로 반환해야 한다.
  - 그렇지 않으면 정의되지 않은 작동을 한다.

#### C 라이브러리의 메모리 관리 함수
- `C++`은 메모리를 할당하는 `C`라이브러리 함수 `malloc()`, `alloc()`, `realloc()`과 반환하는 함수 `free()`를 제공한다.
- `C`프로그램과의 **호환성**을 위해 제공된다.
- `void* malloc(size_t size)`는 동적 변수의 생명 주기 중 **할당 단계**를 구현한다.
  - `size` 바이트를 저장하기에 충분한 저장 공간을 가리키는 포인터를 반환한다.
  - 저장 공간이 부족하다면 `nullptr`를 반환한다.
- `void free(void* p)`는 동적 변수의 생명 주기 중 **해제 단계**를 구현한다.
  - `p`가 가리키는 저장 공간을 메모리 관리자에게 반환한다.
- `void* calloc(size_t count, size_t size)`는 동적 변수의 생명 주기 중 **할당 단계**를 구현한다.
  - 배열의 요소 개수인 `count`와 각 요소의 바이트 크기인 `size`를 곱해 전체 요소의 바이트 크기로 변환한뒤 `malloc()`을 호출한다.
- `void* realloc(void* p, size_t size)`는 메모리 블록의 크기를 바꾸고 필요에 다라 블록을 새 저장 공간으로 옮긴다.
  - 기본 블록의 내용은 새 블록 크기와 기존 블록 크기의 최소값까지 새 블록에 복사된다.
  - 매우 조심해서 사용해야 한다.
  - 포인터가 가리키는 블록을 옮기고 기존 블록을 삭제할수 있는데 이러면 기존 블록을 가리키는 포인터가 무효화된다.

### 13.1.3 new 표현식은 동적 변수를 생성합니다
#### new 표현식
- C++ 프로그램은 `new` 표현식을 사용해 동적 변수나 동적 배열의 생성을 요청한다.
- 키워드 `new`, 뒤이어 나오는 타입, 반환된 값을 가리키는 포인터를 포함한다.
- 변수나 배열 요소의 초기값을 설정할 수 있는 **이니셜라이저**를 포함할 수 있다.
- 완전히 초기화된 `C++` 변수나 배열을 가리키는 타입이 있는 포인터를 반환한다.
- 동적 변수나 동적 배열의 첫 번째 요소를 가리키는 **우측값 포인터**를 반환한다.
- `operator new()` 함수를 호출해 저장 공간을 할당하는 것 이상의 기능을 수행한다.
  - `operator new()`를 성공적으로 호출했다면 배열이 아닌 버전은 `type` 객체를 생성한다.
  - 생성자가 예외를 던진다면 멤버와 베이스는 파괴되고 `operator delete()`를 호출해 할당된 메모리를 반환한다.
- `operator delete()`의 시그니처는 메모리 할당에 사용한 `operator new()`함수와 일치한다.
  - `operator delete()`가 없다면 메모리를 반환하지 않으므로 누수의 위험이 있다.
- 포인터를 반환하거나 잡은 에외를 다시 던지거나 예외를 던지지 않을 경우 `nullptr`를 반환한다.

#### 던지지 않는 new
- `placement-params`가 `std::nothrow` 태그로 구성되었다면 `std::bad_alloc`을 던지지 않는다.
- 객체를 생성하지 않고 `nullptr`를 반환한다.

#### 메모리 지정 new는 할당 없이 메모리를 지정합니다
- `placement-params`가 기존의 유효한 저장 공간을 가리키는 포인터라면 `new` 표현식은 메모리 관리자를 호출하지 않고 포인터가 가리키는 위치에 `type`만 지정한다.
- 포인터는 `type`을 저장할 만큼 충분한 저장공간을 가리켜야 한다.
```cpp
char mem[1000];
class Foo{...};
Foo* foo_p = new (mem) Foo(123);
```
- 클래스 `Foo`의 인스턴스를 배열 `mem`위에 배치한다.
- 메모리 지정 `new`는 클래스의 생성자를 호출해 클래스 인스턴스의 초기화를 수행한다.
- 기본 타입의 경우 생성자를 호출하는 대신 초기화를 수행한다.
- 저장 공간을 할당하지 않으므로 상응하는 메모리 지정 `delete`는 존재하지 않는다.

#### 사용자 정의 메모리 지정 new: 형태를 반쯤 갖춘 할당
- `placement-params`가 `std::nothrow`나 단일 포인터가 아닌 다른 것이라면 **사용자 정의 메모리 지정 new**를 호출한다.
- C++은 사용자 정의 **메모리 지정 new 표현식**에 의미를 부여하지 않는다.
  - 개발자는 이를 지정되지 않은 방법으로 저장 공간을 할당하는 데 사용할 수 있다.
- 사용자 정의 **메모리 지정 new 표현식**은 첫 번째 인수가 `size_t`와 일치하고 이후 인수들이 표현식 목록의 타입들과 일치하는 `operator new()` 또는 `operator new[]()`의 오버로드를 검색한다.
- 동적 객체의 생성자가 **예외**를 던지면 메모리 지정 `new` 표현식은 첫 번째 매개변수가 `void*` 이고 이후 인수들이 표현식 목록의 타입들과 일치하는 `operator delete()` 또는 `operator delete[]()`의 오버로드를 검색한다.
- 사용자 정의 메모리 지정 `new`의 문제는 일치하는 사용자 정의 메모리 지정 `delete`를 지정할 수 있는 방법이 없다는 것이다.
  - 객체의 생성자가 `new`표현식에서 예외를 던질때 `operator delete()`의 다양한 메모리 지정 오버로드가 호출된다.
  - `delete`표현식은 이 오버로드들을 호출할 수 없다.

#### 클래스 한정 operator new()는 할당을 세부적으로 제어할 수 있습니다
- `new` 표현식은 타입이 만들어지는 범위내에서 `operator new()`를 검색한다.
- 클래스 한정 `operator new()`를 구현하면 클래스의 할당을 세부적으로 제어할 수 있다.
  - 정의하지 않으면 **전역 범위의 연산자**를 사용한다.
  - 전역 범위 `operator new()`를 사용하고 싶으면 전역 범위 연산자 `::`를 지정하면 된다.
- 클래스 한정 `operator new()`는 해당 함수를 정의하는 클래스의 **인스턴스를 할당하는 경우**에만 호출된다.
  - 다른 클래스와 관련된 `new`표현식을 포함하는 클래스의 멤버 함수는 다른 클래스에서 정의된 operator new()가 있다면 사용하고 아니라면 전역 범위의 `operator new()`를 사용한다.
- 클래스 한정 `operator new()`는 단일 크기의 객체를 할당하므로 효율적일 수 있다.
  - 첫 번째 자유 블록을 항상 사용할 수 있다.
  - 클래스가 여러 스레드에서 사용하지 않는다면 내부 자료구조를 스레드 세이프하게 만드는 오버헤드를 생략할 수 있다.
- 클래스 한정 `operator new()`는 정적 멤버 함수로 정의된다.
- 클래스가 사용자 정의 `operator new()`를 구현할 경우 해당 `operator delete()`를 구현해야 한다.
  - 구현하지 않으면 전역 `operator delete()`를 호출해 정의되지 않은 작동을 하여 원하지 않은 결과가 나오게 된다.

### 13.1.4 delete 표현식은 동적 변수를 없앱니다
- 프로그램은 `delete` 표현식을 사용해 동적 변수가 사용하는 메모리를 메모리 관리자에게 반환한다.
- `delete` 표현식은 동적 변수의 생명 주기에서 변수를 **파괴**하고 이전에 사용하던 메모리를 **해제**하는 마지막 두 단계를 처리한다.
- `delete` 표현식은 키워드 `delete`와 삭제할 변수를 가리키는 포인터를 생성하는 표현식을 포함한다.
- 동적 변수에 잘못된 `delete` 표현식을 사용하면 `C++`표준에서 정의되지 않은 작동을 한다.

### 13.1.5 소멸자를 명시적으로 호출하면 동적 변수를 파괴합니다
- `delete` 표현식을 사용하는 대신 **소멸자를 명시적으로 호출**하면 저장 공간을 해제하지 않고 동적 변수의 파괴 작동만 수행할 수 있다.
```cpp
foo_p->~Foo();
```
- 소멸자를 명시적으로 호출하는 위치는 메모리 지정 `new`를 호출하는 위치와 같다.
- 표준 라이브러리의 템플릿 `Allocator`는 파괴와 해제가 별개로 일어난다.

#### 생성자를 명시적으로 호출할 방법이 있을까
- 프로그램은 생성자를 직접 호출할 수 없다.
- 생성자는 `new` 표현식을 사용해 호출된다.
- 프로그램이 **이미 생성된 클래스 인스턴스에서 생성자를 명시적으로 호출하는것**은 간단하다.
```cpp
class Blah{
public:
    Blah() {...}
};

Blah * b = new char[sizeof(Blah)];
Blah myBlah;

new (b) Blah;
new (&myBlah) Blah;

b->Blah::Blah();
```

## 13.2 고성능 메모리 관리자
- 저장 공간의 모든 요청은 `::operator new()`를 거쳐가고 해제된 저장 공간은 `::operator delete()`를 거쳐간다.
  - 이런 함수들은` C++`의 **기본 메모리 관리자**를 형성한다.
- 대부분의 `C++` 컴파일러에서 제공하는 `::operator new()`는 C함수` malloc()`을 래핑한 함수이다.

#### C++의 기본 메모리 관리자가 충족해야 할 요구 사항
- 자주 실행될 가능성이 있으므로 **효율적인 성능**을 발휘해야 한다.
- **멀티스레드 프로그램**에서 올바르게 작동해야 한다.
  - 기본 메모리 관리자에서 자료구조에 접근하는 작동이 **직렬화**되어 있어야 한다.
- 크기가 같으면서 개수가 많은 객체(노드 등)들을 효율적으로 할당해야 한다.
- 크기가 다르면서 개수가 많은 객체(문자열 등)들을 효율적으로 할당해야 한다.
- 매우 큰 자료구조(I/O 버퍼)와 작은 자료구조(단일 포인터)를 할당해야 한다.
- 효율성을 극대화 하려면 포인터, 캐시 라인, 가상 메모리 페이지, 최소한 더 크게 할당된 메모리 블록에 대한 **맞춤 경계**를 알고 있어야 한다.
- 시간이 지나면서 런타임 성능이 저하되어선 안된다.
- 반환된 메모리를 효율적으로 재사용해야 한다.

#### malloc()을 대체할 수 있는 고성능 라이브러리
- `Hoard`
  - 멀티 프로세서 메모리 할당자의 상용화 버전이다.
  - `malloc()`보다 성능이 3~7배 향상되었다고 주장한다.
- `mtmalloc`
  - 솔라리스에서 고도의 멀팊스레드 방법인 워크로드를 대체하기 위한 `malloc`이다.
  - **최적 적합 할당자**를 사용한다.
- `ptmalloc`(glibc malloc)
  - 리눅스 3.7 버전 이후에서 함께 제공되는 `malloc`이다.
  - 멀티스레드 프로그램에서 경합을 줄이기 위해 스레드마다 아레나가 있다.
- `TCMalloc`(Thread-Caching malloc())
  - 구글에서 만든 `malloc()` 대체 라이브러리이다.
  - 크기가 작은 객체를 위해 특수화된 할당자와 대규모 블록을 관리하기 위해 설계된 스핀락이 있다.
  - 리눅스에서만 사용할 수 있다.

#### 최신 메모리 관리자를 잘 사용하지 않는 이유
- 성능을 비교할 때 기준이 되는 메모리 관리자가 무엇인지 명확하게 나타내지 않으면 비현실적인 허수아비가 될 수 있다.
  - 윈도우 7, 리눅스 3.7 이후 메모리 관리자를 바꾸더라도 성능이 향상되지 않을 수 있다.
- **동적 변수의 할당 및 해제가 프로그램 실행 시간을 좌우할 경우**에만 성능 향상에 도움이 된다.
- 메모리 관리자를 호출하는 횟수를 줄이면 할당자의 속도와 관계없이 성능을 향상할 수 있다.
- 최신 메모리 관리자는 성능을 향상하기 위해 캐시 및 자유 블록 풀에 메모리를 소비한다.
  - 제약 조건이 있는 환경에서는 메모리를 추가로 사용할 수 없을 수도 있다.

## 13.3 클래스 한정 메모리 관리자 제공하기
- `operator new()`는 클래스 수준에서 재정의 할 수 있다.
- 클래스의 인스턴스를 동적으로 생성하는 코드가 자주 실행된다면 클래스 한정 메모리 관리자로 성능을 향상할 수 있다.
- 클래스에 `operator new()`를 구현하면 인스턴스를 할당할 메모리를 요청할 떄 전역 범위의 `operator new()`대신 구현한 `operator new()`를 호출한다.
- 클래스 한정 `operator new()`는 기본 버전에서 사용할 수 없는 부가적인 지식을 활용할 수 있다.
  - 특정 클래스의 인스턴스를 할당할 메모리를 요청할 때 **요청하는 바이트 수가 모두 같다.**
- 같은 크기를 요청하는 메모리 관리자는 쉽게 만들 수 있고 효율적으로 실행되는데 그 이유는 아래와 같다.
  - 고정된 크기의 블록을 갖는 메모리 관리자는 반환된 메모리를 **효율적으로 재사용**할 수 있다.
  - 똑같은 크기를 요청하기 때문에 **파편화**가 발생하지 않는다.
  - **메모리 오버헤드**가 낮거나 없는 고정된 크기의 블록을 갖는 메모리 관리자를 구현할 수 있다.
  - **메모리 소비량의 상한값을 보장**할 수 있다.
  - 메모리 관리자의 함수가 내부적으로 매우 간단하므로 인라인화 할 수 있다.
  - 캐시 히트율이 높다.

### 13.3.1 고정된 크기의 블록을 갖는 메모리 관리자
- 아래 코드는 **고정된 크기의 블록을 갖는 메모리 관리자**를 정의한다.
```cpp
template <class Arena> struct fixed_block_memory_manager {
  template <int N>
  fixed_block_memory_manager(char(&a)[N]);
  
  fixed_block_memory_manager(fixed_block_memory_manager&) = delete;
  ~fixed_block_memory_manager() = default;
  void operator=(fixed_block_memory_manager&) = delete;

  void* allocate(size_t);
  size_t block_size() const;
  size_t capacity() const;
  void clear();
  void deallocate(void*);
  bool empty() const;
private:
  struct free_block{
    free_block* next;
  };
  free_block* free_ptr_;
  size_t block_size_;
  Arena arena_;
};
```
- 내부구조는 사용 가능한 메모리 블록의 **단일 연결 리스트**만 존재한다.
- 이 메모리 관리자는 아레나라고 하는 정적으로 선언된 저장 공간의 조각에서 블록을 할당한다.

#### 생성자
```cpp
template <class Arena>
  template <int N>
    inline fixed_block_memory_manager<Arena>
    ::fixed_block_memory_manager(char(&a)[N]) :
      arena_(a), free_ptr_(nullptr), block_size_(0) { }
```
- 생성자는 인자로 **C 스타일의 char 배열**을 받는다.
- 이 배열은 메모리 블록을 할당할 아레나를 형성한다.
- 생성자는 템플릿 함수로 배열의 크기를 템플릿 매개변수로 캡처할 수 있다.

#### allocate 함수
```cpp
template <class Arena>
  inline void* fixed_block_memory_manager<Arena>::allocate(size_t size)
  {
    if(empty()){
      free_ptr = reinterpret_cast<free_block*>(arena_.allocate(size));
      block_size_ = size;
      if(empty())
      {
        throw std::bad_alloc();
      }
    }
    if (size != block_size_)
    {
      throw std::bad_alloc();
    }
    auto p = free_ptr_;
    free_ptr_ = free_ptr_->next;
    return p;
  }
```
- `allocate()`는 **사용 가능한 블록이 있는 경우** 빈칸 목록에서 블록을 꺼낸 뒤 반환한다.
- 빈칸 목록이 비어있다면 아레나 관리자에서 새 자유 블록의 목록을 얻으려고 시도한다.
- 더 할당할 메모리가 없다면 `nullptr`를 반환하고 `std::bad_alloc`을 던진다.

#### deallocate 함수
```cpp
template <class Arena>
  inline void fixed_block_memory_manager<Arena>::deallocate(void* p){
    if (p == nullptr)
      return;
    auto fp = reinterpret_cast<free_block*>(p);
    fp->next = free_ptr_;
    free_ptr_ = fp;
  }
```
- 블록을 빈칸 목록으로 밀어 넣는다.

#### capacity, clear 함수
```cpp
template <class Arena>
inline size_t fixed_block_memory_manager<Arena>::capacity() const{
  return arena_.capacity();
}

template <class Arena>
inline void fixed_block_memory_manager<Arena>::clear(){
  free_ptr_ = nullptr;
  arena_.clear();
}
```
- 나머지 멤버 함수들은 간단하게 작성할 수 있다.
- `C++11` 문법을 사용해 클래스 정의에서 메모리 관리자의 복사 및 대입을 사용할 수 없도록 설정하였다.

### 13.3.2 블록 아레나
- `fixed_block_memory_manager`의 복잡성은 처음에 빈칸 목록을 만드는 방법에서 발생한다.
- 이 복잡성은 별도의 템플릿 클래스로 반영하며 이름은 `fixed_arena_controller`로 하였다.
```cpp
struct fixed_arena_controller {
  template <int N>
  fixed_arena_controller(char(&a)[N]);

  fixed_arena_controller(fixed_arena_controller&) = delete;
  ~fixed_arena_controller() = default;
  void operator=(fixed_arena_controller&) = delete;

  void* allocate(size_t);
  size_t block_size() const;
  size_t capacity() const;
  void clear();
  bool empty() const;

private:
  void* arena_;
  size_t arena_size_;
  size_t block_size_;
}
```
- 아레나는 어떤 활동이 일어나는 밀폐된 공간을 의미한다.
- `fixed_arena_controller`는 **고정된 크기의 노드**를 할당할 수 있는 **단일 정적 메모리 블록**을 제공한다.

#### 생성자
```cpp
template <int N>
  inline fixed_arena_controller::fixed_arena_controller(char(&a)[N])
    : arena_(a), arene_size_(N), block_size_(0) { }
```
- 생성자는 아레나 배열을 인자로 받으며 **배열의 크기와 시작 주소를 가리키는 포인터를 저장**한다.

#### allocate 함수
```cpp
inline void* fixed_arena_controller::allocate(size_t size){
  if (!empty())
    return nullptr;

  block_size_ = std::max(size, sizeof(void*));
  size_t count = capacity();

  if (count == 0)
    return nullptr;

  char* p;
  for (p = (char*)arena_; count > 1; --count, p += size){
    *reinterpret_cast<char**>(p) = p + size;
  }
  *reinterpret_cast<char**>(p) = nullptr;
  return arena_;
}
```
- `fixed_arena_controller`는 할당된 단일 메모리 블록을 갖는다.
  - 해당 블록을 모두 사용했다면 `allocate()`를 **다시 호출**하고 오류를 나타내는값(`nullptr`)을 반환한다.
- 처음 호출할 때 블록의 크기와 블록 단위의 용량을 설정한다.
- `fixed_arena_controller`는 아레나 배열의 크기를 제어할 수 없다.
  - 배열 끝부분의 몇 바이트는 한 번도 할당하지 않고 사용하지 않을 수 있다.
- `fixed_arena_controller`의 할당 및 해제 코드는 간단하다.
  - 생성자에게 제공된 저장 공간 위에 사용 가능한 노드 목록을 중첩하게 만든다.
  - 목록의 첫 번째 요소를 가리키는 포인터를 반환한다.

### 13.3.3 클래스 한정 operator new() 추가하기
- 아래는 클래스 한정 `operator new()`와 `operator delete()`가 있는 매우 간단한 클래스이다.
```cpp
class MemMgrTester{
  int contents_;
public:
  MemMgrTester(int c) : contents_(c) {}

  static void* operator new(size_t s){
    return mgr_.allocate(s);
  }

  static void operator delete(void* p){
    mgr_.deallocate(p);
  }
  static fixed_block_memory_manager<fixed_arena_controller> mgr_;
}
```
- 고정된 크기의 블록을 갖는 메모리 관리자인 정적 멤버 `mgr_`을 포함하고 있다.
- `operator new()`와 `operator delete()`는 `mgr_`의 멤버 함수 `allocate()`와 `deallocate()`에 요청 사항을 전달하는 **인라인 함수**다.
- `mgr_`를 `public`으로 선언했으므로 `mgr_.clear()`를 호출해서 **빈칸 목록을 다시 초기화**할 수 있다.
- 이렇게 재설정할 수 있는 메모리 관리자를 **풀 메모리 관리자**라고 하며 이를 제어하는 아레나를 메모리 풀이라고 한다.
  - 풀 메모리 관리자는 자료구조를 생성, 사용, 파괴하는 경우에 유용하다.
- 메모리 풀 전체를 빠른 속도로 초기화 할 수 있다면 프로그램은 자료구조를 노드마다 해제하지 않아도 된다.

```cpp
char arena[4004];
fixed_block_memory_manager<fixed_arena_controller>
  MemMgrTester::mgr_(arena);
```
- `mgr_`는 클래스 정적 멤버로 선언된다.
  - 프로그램의 어딘가에서는 정의해야 한다.
- 메모리 아레나와 `mgr_`을 정의하는 코드이다.
- 생성자는 아레나를 인수로 받는다.

### 13.3.4 고정된 크기의 블록을 갖는 메모리 관리자의 성능
- 고정된 크기의 블록을 갖는 메모리 관리자는 매우 효율적이다.
- 할당 및 해제 코드는 고정된 비용을 가지며 코드를 인라인화 할 수 있다.
- `malloc()`보다 빠른 성능을 보인다.

### 13.3.5 고정된 크기의 블록을 갖는 메모리 관리자의 변형
- 고정된 크기의 블록을 갖는 메모리 관리자의 기본 구조는 매우 간단하다.
- 이 구조를 변형해 최적화 작업 중인 프로그램에 더 적합한 게 있는지 확인해 볼 수 있다.
- 빈칸 목록이 비어있을 때 고정된 크기를 갖는 아레나로 시작하는 대신 `malloc()`을 사용해 메모리를 할당할 수 있다.
  - 해제된 메모리 블록은 빠르게 재사용할 수 있도록 빈칸 목록에 캐시된다.
- 아레나가 고정된 크기를 갖는 대신 `malloc()`이나 `::new`를 호출해 만들 수 있다.
- 클래스의 인스턴스를 잠시 사용한 뒤 모두 버린다면 고정된 크기의 블록을 갖는 메모리 관리자는 메모리 풀로 사용할 수 있다.
  - 메모리 풀에서 할당은 정상적으로 진행되지만 메모리는 전혀 해제되지 않는다.
- 일반 메모리 관리자는 서로 다른 아레나에서 서로 다른 크기 요청을 처리하고 서로 다른 크기 요청을 서로 다른 빈칸 목록으로 반환하도록 설계할 수 있다.

### 13.3.6 스레드 세이프하지 않은 메모리 관리자는 효율적입니다
- 고정된 크기의 블록을 갖는 메모리 관리자가 효율적인 이유 중 하나는 스레드 세이프하지 않기 때문이다.
- 스레드 세이프하지 않은 메모리 관리자가 효율적인 이유는 두 가지이다.
  1. **임계 구역**을 직렬화하는 동기화 메커니즘이 필요하지 않다.
  2. **동기화 장치**를 사용하지 않으므로 효율적이다.
- 클래스가 클래스 한정 메모리 관리자를 구현했다면 프로그램 전체가 멀티스레드더라도 주어진 클래스를 하나의 스레드에서만 사용한다면 기다릴 필요가 없다.
- 메모리 관리자를 호출하면 멀티스레드 프로그램에서 하나의 스레드에서만 사용하는 객체에 대해서도 경합이 발생한다.
- 스레드 세이프하지 않은 메모리 관리자는 스레드 세이프한 메모리 관리자보다 쉽게 만들 수 있다.
  - **임계 구역을 최소화**해 메모리 관리자를 효율적으로 실행할 수 있기 때문이다.

## 13.4 사용자 정의 표준 라이브러리 할당자 제공하기
- 표준 라이브러리 컨테이너는 메모리 관리를 사용자가 지정할 수 있도록 인수 `Allocator`를 받는다.
  - `Allocator`는 메모리를 관리하는 템플릿 클래스이다.
  - 메모리 관리자에서 **저장 공간을 검색**한다.
  - 저장 공간을 **메모리 관리자로 반환**한다.
  - 연관된 할당자를 **복사 생성**한다.
- 노드 기반의 컨테이너 중 하나라면 **사용자 정의 할당자**를 구현해 성능을 향상시킬 수 있다.
- 기본 할당자인 `std::allocator<T>`는 `::operator new()`를 둘러싸는 **래퍼**이다.
  - 개발자는 기본 할당자가 아닌 다른 작동을 하는 할당자를 제공할 수 있다.

#### 할당자의 두 가지 종류
- 상태가 없는 할당자
  - **정적 상태가 아닌 할당자 타입**을 말한다.
  - 표준 라이브러리 컨테이너의 기본 할당자인 `std::allocator<T>`는 상태가 없는 할당자이다.
  - 상태가 없는 할당자는 **기본 생성**할 수 있다.
  - 상태가 없는 할당자는 **컨테이너 인스턴스의 공간**을 차지하지 않는다.
  - 상태가 없는 할당자 `my_allocator<T>`의 두 인스턴스를 구별할 수 없다.
  - 이동 대입과 `std::swap()`을 효율적으로 수행할 수 있다.
- 상태가 있는 할당자
  - 만들고 사용하기가 더 복잡하다.
  - 지역 상태를 갖는 할당자는 대부분의 경우 **기본 생성할 수 없다.**
  - 할당자를 생성한 다음 컨테이너의 생성자에게 전달해야 한다.
  - 모든 변수에 할당자의 상태를 저장해야 하므로 **크기가 증가**한다.
  - 같은 타입을 갖는 두 할당자가 같은지 **비교하지 못할 수도 있다.**
  - 전역 메모리 관리자를 통해 모든 요청을 처리하지 않아도 되는 경우 여러 종류의 메모리 아레나를 다양한 용도로 쉽게 만들 수 있다.

### 13.4.1 미니멀한 C++11 할당자
- `C++11`을 지원하는 컴파일러와 표준 라이브러리를 갖고 있다면 미니멀한 할당자를 제공할 수 있다.
- 아래는 std::allocator에서 수행하는 작업을 대략적으로 나타낸 할당자 코드이다.
```cpp
template<typename T> struct my_allocator {
  using value_type = T;
  my_allocator() = default;

  template<class U> my_allocator(const my_allocator<U>&) {}

  T* allocate(std::size_t n, void const* hint = 0)
  {
    return reinterpret_cast<T*>(::operator new(n * sizeof(T)));
  }

  void deallocate(T* ptr, size_t)
  {
    ::operator delete(ptr);
  }
};

template<typename T, typename U>
inline bool operator==(const my_allocator<T>&, const my_allocator<U>&)
{
  return true;
}

template<typename T, typename U>
inline bool operator!=(const my_allocator<T>& a, const my_allocator<U>& b)
{
  return !(a == b);
}
```
- 미니멀한 할당자는 아래 몇 가지 함수를 포함한다.

#### allocator
- 기본 생성자이다.
- 할당자에 기본 생성자가 있다면 개발자는 인스턴스를 명시적으로 생성해 컨테이너의 생성자에게 전달할 필요가 없다.
- **상태가 없는 할당자**에는 기본 생성자의 본문이 비어있으며 **정적 상태가 아닌 할당자**에서는 기본 생성자가 없다.

#### template <typename U> allocator(U&)
- 이 복사 생성자를 사용하면 `allocator<T>`를 `allocator<treenode<T>>`처럼 **연관**되어 있는 `private` 클래스의 할당자로 변환할 수 있다.
- 이 복사 생성자는 대부분의 컨테이너가 `T`타입의 노드를 할당하지 않기 때문에 중요하다.
- **상태가 없는 할당자**에는 복사 생성자의 본문이 비어있으며 **정적 상태가 아닌 할당자**에서는 상태를 복사하거나 복제해야 한다.

#### T* allocate(std::size_t n, void const* hint = 0)
- `n`바이트를 저장할 수 있는 충분한 저장 공간을 할당한 뒤 저장 공간을 가리키는 포인터를 반환하거나 `std::bad_alloc`을 던진다.
- `hInt`는 **지역성**과 관련해 지정되지 않은 방법으로 할당하는 것을 돕기 위한 인수이다.

#### void deallocate(T* ptr, size_t)
- `allocate()`에서 반환했던 **저장 공간을 해제한 뒤 메모리 관리자에게 반환**한다.
- 인수 `p`는 저장 공간을 가리키며 `n`은 차지하는 바이트를 나타낸다.
- `n`은 `p`가 가리키는 저장 공간을 `allocate()`를 통해 할당했을 때 넣었던 인수값과 같아야 한다.

#### bool operator==(allocator const& a) const와 bool operator!=(allocator const& a) const
- 같은 타입의 두 할당자 인스턴스가 서로 같은지를 검사한다.
- 두 인스턴스가 같다면 하나의 할당자 인스턴스에서 할당된 객체를 다른 할당자 인스턴스에서 **안전하게 해제**할 수 있다.
  - 두 인스턴스가 **같은 저장 공간에 객체를 할당**한다는 것을 의미한다.
- 상태가 없는 할당자는 똑같은지 검사할 때 무조건 `true`를 반환한다.
- 정적인 상태가 아닌 할당자는 똑같은지 검사할 때 할당자의 상태를 비교하거나 `false`를 반환해야 한다.

### 13.4.2 C++98 할당자의 추가 정의
- `C++11` 이전에 모든 할당자는 미니멀한 할당자에 있는 모든 함수뿐만 아니라 다음 함수들을 추가로 포함하고 있다.

#### value_type
- 할당할 객체의 타입

#### size_type
- 이 할당자가 할당할 수 있는 **최대 바이트 수**를 저장할 수 있을 정도로 충분히 큰 **정수 타입**
- 표준 라이브러리 컨테이너 템플릿에서 매개변수로 사용하는 할당자의 경우 `typedef size_t size_type;`으로 정의되어 있어야 한다.

#### deifference_type
- **두 포인터 사이의 최대 차이**를 저장할 수 있을 정도로 충분히 큰 **정수 타입**
- 표준 라이브러리 컨테이너 템플릿에서 매개변수로 사용하는 할당자의 경우 `typedef ptrdiff_t difference_type;`으로 정의되어 있어야 한다.

#### pointer, const_pointer
- `(const) T`를 가리키는 **포인터의 타입**
- 표준 라이브러리 컨테이너 템플릿에서 매개변수로 사용하는 할당자의 경우 두 타입은 아래와 같이 정의되어 있어야 한다.
```cpp
typedef T* pointer;
typedef T const* const_pointer;
```
- 다른 할당자의 경우 `pointer`는 포인터를 역참조하는 `operator()`를 구현한 포인터와 비슷한 클래스일 수 있다.

#### reference, const_reference
- `(const) T`를 가리키는 **참조 타입**
- 표준 라이브러리 컨테이너 템플릿에서 매개변수로 사용하는 할당자의 경우 두 타입은 아래와 같이 정의되어 있어야 한다.
```cpp
typedef T& reference;
typedef T const& const_reference;
```

#### pointer address(reference), const_pointer address(const_reference)
- `(const) T`를 가리키는 **참조**가 주어졌을 때 `(const) T`를 가리키는 **포인터를 생성**하는 함수

#### pointer address(reference r){ return &r; }, const_pointer address(const_reference r){ return &r; }
- 두 함수는 **메모리 모델을 추상화**하기 위한 것이다.
- 표준 라이브러리 컨테이너와의 **호환성을 방해**한다.
- 실제로는 `T*`가 되기 위해서는 `pointer`가 필요하다.
- **임의 접근 반복자**와 **이진 검색**을 효율적으로 수행할 수 있다.
- 표준 라이브러리 컨테이너에서 사용하는 할당자를 위한 고정값들이 있음에도 불구하고 정의가 필요하다.
  - `C++98`의 컨테이너 코드에서 사용하기 때문이다.
```cpp
typedef size_type allocator::size_type;
```
- `std::allocator`는 언젠가 바뀔수도 있다.
  - 초기에는 많이 바뀌었고 `C++11`에서 다시 바뀌었다.
  - 또 바뀔지 모른다는 두려움을 가지는 것이 당연하다.
- 또 다른 접근법은 정의에서 가장 변하지 않는 부분을 간단하게 뽑을 수 있다.
```cpp
template <typenamt T> struct std::allocator_defs {
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  pointer address(reference r) { return &r; }
  const_pointer address(const_reference r) { return &r; }
}
```
- 이 정의를 **특성 클래스**로 만들 수 있다.
- `C++11`의 미니멀한 할당자가 수행하는 작업이며 **특성 클래스**만 역방향으로 작동한다.
- **특성 클래스**는 할당자 템플릿을 살펴보고 정의가 있는지 확인하고 할당자가 없을 경우 표준 정의를 제공한다.
- 컨테이너 코드는 할당자가 아닌 `allocator_traits` 클래스를 참조한다.
```cpp
typedef std::allocator_traits<MyAllocator<T>>::value_type value_type;
```

#### void construct(pointer p, const T& val)
- 메모리 지정 `new`를 사용해 `T`의 인스턴스를 복사 생성한다.
```cpp
new(p) T(val);
```
- `C++11`에서는 이 함수를 정의해 **인수 목록을 T 생성자에게 제공**할 수 있다.
```cpp
template <typename U, typename... Args>
void construct(U* p, Args&&... args){
  new(p) T(std::forward<Args>(args...));
}
```

#### void destroy(pointer p);
- `p->~T();`를 호출해 **T가 가리키는 포인터를 파괴**한다.

#### rebind::value
- 구조체 `rebind`의 선언은 할당자의 핵심이다.
- `rebind`는 보통 다음과 같은 구조를 갖는다.
```cpp
template <typename U> struct rebind {
  typedef allocator<U> value;
};
```
- `rebind`는 `allocator<T>`에서 새 타입 `U`를 위한 **할당자를 만드는 방법을 제공**한다.
  - 모든 할당자는 이 방법을 제공해야 한다.
  - `std::list<T>`와 같은 컨테이너가 `std::list<T>::listnode<T>`의 인스턴스를 할당하는 방법이다.
  - 대부분의 컨테이너에서 타입 `T`의 노드가 할당되지 않는다.
- 아래는 미니멀한 `C++11` 할당자와 같은 `C++98` 스타일 할당자의 전체 코드이다.
```cpp
template <typename T> struct my_allocator_98 :
  public std_allocator_defs<T> {
  template <typename U> struct rebind {
    typedef my_allocator_98<U, n> other;
  };

  my_allocator_98() {/* 비어 있음 */ }
  my_allocator_98(my_allocator_98 const&) {/* 비어 있음 */ }

  void construct(pointer p, const T& t) {
    new(p) T(t);
  }
  void destroy(pointer p) {
    p->~T();
  }
  size_type max_size() const {
    return block_o_memory::blocksize;
  }
  pointer allocate(size_type n, typename std::allocator<void>::const_pointer = 0) {
    return reinterpret_cast<T*>(::operator new(n * sizeof(T)));
  }
  void deallocate(pointer p, size_type) {
    ::operator delete(ptr);
  }
};

template <typename T, typename U>
inline bool operator==(const my_allocator_98<T>&, const my_allocator_98<U>&) {
  return true;
}

template <typename T, typename U>
inline bool operator!=(const my_allocator_98<T>& a, const my_allocator_98<U>& b) {
  return !(a == b);
}
```

### 13.4.3 고정된 크기의 블록을 갖는 할당자
- 표준 라이브러리 컨테이너 클래스(`std::list`, `std::map`, `std::multimap`, `std::set`, `std::multiset`)는 모두 **여러 개의 노드에서 자료구조**를 만든다.
- 이 클래스들은 **고정된 크기의 블록을 갖는 메모리 관리자를 사용해 구현된 할당자**를 사용할 수 있다.
  - `allocate()`함수와 `deallocate()`함수를 제외하곤 모두 동일하다.
```cpp
extern fixed_block_memory_manager<fixed_arena_controller>
  list_memory_manager;

template <typename T> class StatelessListAllocator{
public:
  pointer allocate(
    size_type count,
    typename std::allocator<void>::const_pointer = nullptr){
      return reinterpret_cast<pointer>(list_memory_manager.allocate(count * sizeof(T)));
    }
    void deallocate(pointer p, size_type){
      string_memory_manager.deallocate(p);
    }
};
```
- `std::list`는 `T` 타입의 노드를 할당하려고 시도하지 않는다.
- 템플릿 매개변수 `Allocator`를 사용해 `list_memory_manager.allocate(sizeof(<listnode<T>>))`를 호출해서 `listnode<T>`를 생성한다.
- `std::list`는 다른 리스트 노드와 크기가 다른 특별한 **센티넬 노드**를 할당한다.
  - 센티넬 노드는 일반 리스트 노드보다 작기 때문에 고정된 크기의 블록을 갖는 메모리 관리자가 작동할 수 있도록 수정이 필요하다.
- `allocate()`가 현재 요청한 크기가 저장된 블록 크기와 같은지 검사하는 대신 저장된 블록 크기보다 크지 않은지만 검사하도록 바꾼다.
```cpp
template <class Arena>
inline void* fixed_block_memory_manager<Arena>::allocate(size_t size){
  if(empty()){
    free_ptr_ = reinterpret_cast<free_block*>(arena_.allocate(size));
    block_size_ = size;
    if (empty())
      throw std::bad_alloc();
  }

  if (size > block_size_)
    throw std::bad_alloc();
  auto p = free_ptr_;
  free_ptr_ = free_ptr_->next;
  return p;
}
```
- 고정된 크기의 블록을 갖는 할당자는 기본 할당자보다 5.6배 빠르다.

### 13.4.4 문자열에 대한 고정된 크기의 블록을 갖는 할당자
- `std::string`은 동적으로 할당된 `char` 배열에 내용을 저장한다.
- 문자열의 길이가 길어질수록 배열을 더 큰 크기로 재할당하기 때문에 고정 블록을 갖는 할당자는 적합하지 않아 보인다.
- 프로그램에서 문자열이 취할 수 있는 최대 길이를 안다면 항상 **최대 길이의 고정된 블록을 생성하는 할당자**를 만들 수 있다.
```cpp
template <typename T> class NewAllocator{
public:
  ...
  pointer allocate(
    size_type /*count*/,
    typename std::allocator<void>::const_pointer = nullptr){
      return reinterpret_case<pointer>(string_memory_manager.allocate(512));
  }
  
  void deallocate(pointer p, size_type) {
    ::operator delete(p);
  }
}
```
- `allocate()`가 요청한 크기를 **완전히 무시한 채 고정된 크기의 블록을 반환**한다.
- 성능이 향상되기는 하지만 [다른 최적화 기법](/Chapter/Chapter4.md)이 더 나은 결과를 보여준다.
# Chapter11 입출력 최적화
## 11.1 파일을 읽는 방법
#### 텍스트 파일을 읽어 문자열로 저장하는 함수
- 주로 `XML`이나 `JSON` 블록으로 된 **문자열을 파싱**하기 위한 목적으로 사용된다.
- 이 장에서 최적화를 위해 사용할 예제이다.
```cpp
std::string file_reader(char const* fname)
{
    std::ifstream f;
    f.open(fname);
    if (!f)
    {
        std::cout << "Can't open " << fname << " for reading" << std::endl;
        return "";
    }

    std::stringstream s;
    std::copy(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(s));
    return s.str();
}
```
- `fname`에는 **파일 이름**이 저장되어 있다.
- `file_reader()`는 파일을 열 수 없을 경우 표준 출력에 오류 메시지를 출력하고 빈 문자열을 반환한다.
- 파일을 열었다면 `std::copy()`는 `f`의 스트림 버퍼를 `std::stringstream s`의 스트림 버퍼로 복사한다.

### 11.1.1 저렴한 함수 시그니처 만들기
- 라이브러리 설계 관점에서 보면 `file_reader()`를 개선할 수 있다.
- `file_reader()`는 여러가지 일을 하기 때문에 라이브러리 함수로 사용하기는 어렵다.
  - 파일을 열지 못할 경우 오류 메시지를 출력한다.
  - 파일을 열었다면 파일 스트림 버퍼를 문자열 스트림 버퍼로 읽는다.
- `file_reader()`는 새 메모리 공간을 만들고 반환한다.
  - 잠재적으로 여러 복사본을 반환값으로 만드는 패턴을 호출 체인에 전달한다.
- 빈 문자열을 반환하는 두 가지 경우를 구별할 수 있는 오류 메시지가 필요하다.
  - 파일을 **열지 못할 경우 빈 문자열**을 반환한다.
  - 파일을 읽을 수 있더라도 **아무 내용이 없다면 빈 문자열**을 반환한다.

#### 파일을 열고 스트림을 읽는 부분을 따로 빼서 구현한 코드
```cpp
void stream_read_streambuf_stringstream(std::istream& f, std::string& result)
{
    std::stringstream s;
    std::copy(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(s));
    s.str().swap(result);
}

// 클라이언트에서 사용하는 코드
std::string fname = "파일 이름";
std::string s;
std::ifstream f;
f.open(fname);
if (!f)
{
  std::cerr <<"Can't open " <<fname << " for reading" << stD::endl;
}
else
{
  stream_read_streambuf_stringstream(f, s);
}
```
- `swap`을 통해 할당하고 복사하는 것보다 비용이 적게 든다.

#### 스트림 반복자를 복사하는 또 다른 파일 리더
```cpp
void stream_read_streambuf_string(std::istream& f, std::string& result)
{
    result.assign(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>());
}
```
- `stream_read_streambuf_stringstream()`는 문자 반복자를 사용하여 **한 번에 한 문자씩 복사**한다.
- `std::string::assign()`을 사용해 반복자를 입력 스트림에서 `std::string`으로 복사하였다.

### 11.1.2 호출 체인 짧게 만들기
- `std::istream`의 `operator<<()`를 사용해 **한 번에 한 문자씩 복사**하는 코드이다.
```cpp
void stream_read_streambuf(std::istream& f, std::string& result)
{
    std::stringstream s;
    s << f.rdbuf();
    s.str().swap(result);
}
```
- `stream_read_streambuf_string`보다 빠른 성능을 보인다.
- 실제 실행 되는 코드가 적기 때문일 것이다.

### 11.1.3 재할당 줄이기
- 파일에서 읽은 내용을 저장하는 `std::string`에 `reserve()`를 호출해 미리 할당하면 한 문자씩 저장해 문자열이 길어져 발생하는 비용이 큰 재할당을 방지할 수 있다.
```cpp
void stream_read_string_reserve(std::istream& f, std::string& result)
{
    f.seekg(0, std::istream::end);
    std::streamoff len = f.tellg();
    f.seekg(0);
    if (len > 0)
    {
        result.reserve(static_cast<std::string::size_type>(len));
    }
    result.assign(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>());
}
```
- `stream_read_streambuf_string`보다는 나은 성능을 보이지만 눈에 띌 정도는 아니다.

#### stream_read_string()의 범용 버전
```cpp
void stream_read_string_2(std::istream& f, std::string& result, std::streamoff len = 0)
{
    if (len > 0)
    {
        result.reserve(static_cast<std::string::size_type>(len));
    }
    result.assign(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>());
}
```

#### 스트림 길이를 계산하는 함수
- 스트림 길이를 찾고 저장 공간을 미리 할당하는 기법은 매우 유용하다.
```cpp
std::streamoff stream_size(std::istream& f)
{
    std::istream::pos_type current_pos = f.tellg();
    if (-1 == current_pos)
    {
        return -1;
    }
    f.seekg(0, std::istream::end);
    std::istream::pos_type end_pos = f.tellg();
    f.seekg(current_pos);
    return end_pos - current_pos;
}
```

### 11.1.4 더 큰 입력 버퍼 사용하기
- `C++` 스트림은 `std::streambuf`에서 파생된 클래스를 포함하는데 운영체제에서 데이터를 더 큰 덩어리로 읽어 파일 읽기 성능을 향상한다.
- 데이터는 스트림 버퍼의 내부 버퍼에 저장되며 앞에서 설명했던 반복자 기반 입력 방법으로 바이트 단위로 추출된다.
- 입력 버퍼의 크기가 증가하면 성능이 약 5% 정도 향상된다. 크기가 `8KB`를 넘어가면 별 다른 영향을 미치지 않는다.

### 11.1.5 한 번에 한 줄씩 읽기
- 행으로 나눠진 텍스트 파일이라면 행을 읽는 함수를 사용해 호출 횟수를 줄이는 방법을 생각해볼 수 있다.
- 표준 라이브러리에는 `getline()`이라는 함수가 존재한다.

#### getline을 사용한 코드
```cpp
void stream_read_getline(std::istream& f, std::string& result)
{
    std::string line;
    result.clear();
    while (getline(f, line))
    {
        (result += line) += "\n";
    }
}
```
- `result`의 크기가 이미 재할당하지 않아도 될 정도로 클 수 있지만 만약을 대비해 공간을 확보하는게 좋다.
- `stream_read_string_2()`를 `stream_read_getline()`에 추가할 수 있다.

#### 한 번에 한 줄씩 읽으며 결과 변수를 미리 할당하는 코드
```cpp
void stream_read_getline_2(std::istream& f, std::string& result, std::streamoff len = 0)
{
    std::string line;
    result.clear();

    if (len > 0)
    {
        result.reserve(static_cast<std::string::size_type>(len));
    }

    while (getline(f, line))
    {
        (result += line) += "\n";
    }
}
```
- `stream_read_getline()`보다 성능이 3% 향상된다.

#### std::streambuf의 멤버 함수 sgetn()을 사용하는 방법
- `sgetn()`은 데이터를 임의의 길이만큼 검색해 호출할 때 지정한 버퍼 인수로 가져온다.
- 파일의 크기가 적당하다면 **파일 전체를 한 번에 검색**할 수 있다.

```cpp
bool stream_read_sgetn(std::istream& f, std::string& result)
{
    std::streamoff len = stream_size(f);
    if (len == 01)
    {
        return false;
    }

    result.resize(static_cast<std::string::size_type>(len));

    f.rdbuf()->sgetn(&result[0], len);
    return true;
}
```
- `stream_read_streambuf()`보다 4배 빠른 성능을 보인다.

### 11.1.6 다시 호출 체인 짧게 만들기
#### 문자열의 저장 공간에 read를 사용하는 코드
```cpp
bool stream_read_string(std::istream& f, std::string& result)
{
    std::streamoff len = stream_size(f);
    if (len == -1)
    {
        return false;
    }
    result.resize(static_cast<std::string::size_type>(len));
    f.read(&result[0], result.length());
    return true;
}
```
- `stream_read_sgetn()`보다 약 25% 빠르며 `file_reader()`보다 5배 빠른 성능을 보인다.

#### 문자열의 저장 곤간이 연속적이지 않더라도 사용할 수 있는 코드
- `stream_read_sgetn()`과 `stream_read_string()`의 문제는 포인터 &s[0]가 연속된 저장 공간을 가리킨다고 가정한다.
- `C++11` 이전 표준에서는 문자열의 문자를 연속적으로 **저장하지 않아도 되지만** 모든 표준 라이브러리는 이 방법으로 구현되었다.
- 동적으로 할당된 문자 배열에 데이터를 읽은 뒤 `assign()`을 사용해 문자열로 복사한다.

```cpp
bool stream_read_array(std::istream& f, std::string& result)
{
    std::streamoff len = stream_size(f);
    if (len == -1)
    {
        return false;
    }

    std::unique_ptr<char> data(new char[static_cast<size_t>(len)]);
    f.read(data.get(), static_cast<std::streamsize>(len));
    result.assign(data.get(), static_cast<std::string::size_type>(len));
    return true;
}
```
- `stream_read_string()`보다는 약간 느린 성능을 보인다.
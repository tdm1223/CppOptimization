## Chapter5 알고리즘 최적화
- 몇 초 만에 실행되어야 하는 프로그램이 몇 시간이 걸린다면 선택해야할 최적화는 더 효율적인 알고리즘을 선택하는 것이다.

## 5.1 알고리즘의 시간 비용
- `O(1)` 또는 상수
  - 가장 빠른 알고리즘. 입력값에 상관없는 고정 비율이다.
- `O(logN)`
  - 여러번 호출해도 괜찮다. 대표적인 예로는 이진 검색 알고리즘
- `O(N)` 또는 선형 시간
  - 알고리즘을 수행하는 데 걸리는 시간은 입력값에 비례한다.
- `O(NlogN)`
  - 입력값이 커지는 속도보다 시간 비용이 증가하는 속도가 빠른 알고리즘.
  - 입력 값이 크더라도 대처 가능한 수준이다.
- `O(N`<sup>2</sup>`)`, `O(N`<sup>3</sup>`)` ...
  - 입력값이 커지는 속도보다 시간 비용이 증가하는 속도가 매우 빠르다.
  - 입력값이 크다면 사용을 고려해야 한다.
- `O(2`<sup>n</sup>`)`
  - 입력값이 커지는 속도보다 시간 비용이 증가하는 속도가 매우 빠르다.
  - 입력값이 작은 경우에만 사용해야 한다.
  - 대표적인 예로는 `TSP` 알고리즘이 있다.

### 5.1.1 최선의 경우, 최악의 경우, 평균의 경우 시간 비용
- 알고리즘은 입력값의 순서에 따라 속도가 정해지므로 성능이 중요한 코드에서 어떤 알고리즘을 사용할지 정할 때는 **최악**을 고려해야 한다.
- 입력값이 정렬되어있다는 사실을 알고 있다면 **최선의 경우**에서 좋은 성능을 보이는 알고리즘을 선택해 프로그램의 실행 시간을 향상시킬 수 있다.

### 5.1.2 상환 시간 비용
- 입력값이 클 때 **전체 시간 비용**을 **입력값**으로 나눈 **평균 시간 비용**

### 5.1.3 기타 비용
- 어떤 알고리즘은 **중간 결과를 저장**해 실행 속도를 높일 수 있다.
  - 중간 저장 비용이 발생할 수 있다.
  - 저장 공간 비용이 큰 알고리즘은 제한된 환경에서 사용하지 못할 수도 있다.
- 어떤 알고리즘은 **병렬화**로 실행 속도를 높일 수 있다.
  - 병렬로 실행 가능한 프로세서가 많아야 한다.

## 5.2 검색과 정렬을 최적화하는 툴킷
1. 평균의 경우에 `big-O` 시간 비용이 많은 알고리즘을 시간 비용이 더 적은 알고리즘으로 바꿀 수 있다.
2. 데이터가 어떤 특징을 가진다면 해당 특징을 활용해 가장 좋은 `big-O` 시간 비용을 갖는 알고리즘을 선택할 수 있다.
3. 알고리즘을 수정해 성능을 향상할 수 있다.

## 5.3 효율적인 검색 알고리즘
### 5.3.1 검색 알고리즘의 시간 비용
1. 선형 검색
  - 시간 비용 : `O(N)`
  - 가장 일반적인 검색 알고리즘
  - 정렬되지 않은 테이블에서 사용할 수 있다.
  - 키 입력 순서에 상관없이 작동한다.
2. 이진 검색
  - 시간 비용 : `O(logN)`
  - 입력 데이터가 `키`를 기준으로 정렬되어 있어야 한다.
  - 키와 테이블의 중간 위치에 있는 요소를 비교해 앞에 있는지 뒤에 있는지 판별한 뒤 테이블을 절반으로 나눈다.
3. 보간 검색
  - 이진 검색과 같이 정렬된 테이블을 두 부분으로 나눈다.
  - 조금 더 효율적으로 분할하기 위해 키의 부가적인 정보를 사용한다.
  - 키가 균일하게 분포되어 있을때 시간 비용 : `O(loglogN)`
  - 테이블이 크거나 테이블 요소를 테스트하는 데 드는 비용이 중요하다면 성능이 매우 향상 될 수 있다.
4. 해싱
  - 키를 해시 테이블의 배열 색인으로 변환한다.
  - 시간 비용 : `O(1)`
  - 최악의 경우 `O(N)`

### 5.3.2 모든 검색 알고리즘은 n이 작으면 같습니다
- 테이블이 **작으면** 모든 알고리즘이 같은 수의 항목을 테스트하게 된다.
- 테이블이 **커지면** 차이가 발생하게 된다.
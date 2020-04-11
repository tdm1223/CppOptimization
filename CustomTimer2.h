#include <ctime>
class CustomTimer2
{
public:
    // 타이머 생성자
    CustomTimer2()
    {
        start = -1; 
    }

    // 타이머 삭제
    void Clear() 
    {
        start = -1;
    }

    // 타이머 실행 유무
    bool IsStarted() const
    {
        return (start != -1);
    }

    // 타이머 시작
    void Start()
    {
        start = clock();
    }

    // 타이머가 시작한 이후 경과한 시간을 얻음 (밀리초 단위)
    unsigned long GetMs()
    {
        clock_t now;
        if (IsStarted())
        {
            now = clock();
            clock_t dt = (now - start);
            return (unsigned long)(dt * 1000 / CLOCKS_PER_SEC);
        }
        return 0;
    }
private:
    clock_t start;
};
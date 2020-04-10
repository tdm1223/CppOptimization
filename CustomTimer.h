#include <chrono>

using namespace std::chrono;

class CustomTimer
{
public:
    // 타이머 생성자
    CustomTimer() : startTime_(system_clock::time_point::min()) {}

    // 타이머 삭제
    void Clear()
    {
        startTime_ = system_clock::time_point::min();
    }

    // 타이머 실행 유무
    bool IsStarted() const
    {
        return (startTime_.time_since_epoch() != system_clock::duration(0));
    }

    // 타이머 시작
    void Start()
    {
        startTime_ = system_clock::now();
    }

    // 타이머가 시작한 이후 경과한 시간을 얻음 (밀리초 단위)
    unsigned long GetMs()
    {
        if (IsStarted())
        {
            system_clock::duration diff;
            diff = system_clock::now() - startTime_;
            return (unsigned)(duration_cast<milliseconds>(diff).count());
        }
        return 0;
    }
private:
    system_clock::time_point startTime_;
};
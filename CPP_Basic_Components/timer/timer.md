```cpp
#include <chrono>
using namespace std;
using namespace std::chrono;

class Timer
{
private:
    time_point<high_resolution_clock> m_begin;

public:
    Timer() : m_begin(high_resolution_clock::now()) {}

    void reset() {
        m_begin = high_resolution_clock::now();
    }

    template<typename Duration = milliseconds>
    int64_t elapsed() const {
        return duration_cast<Duration>(high_resolution_clock::now() - m_begin).count();
    }

    //微秒
    int64_t elapsedMicro() const {
        return elapsed<microseconds>();
    }

    //纳秒
    int64_t elapsedNano() const {
        return elapsed<nanoseconds>();
    }

    //秒
    int64_t elapsedSesonds() const {
        return elapsed<seconds>();
    }

    //分
    int64_t elapsedMinutes() const {
        return elapsed<minutes>();
    }

    //时
    int64_t elapsedHours() const {
        return elapsed<hours>();
    }
};
```
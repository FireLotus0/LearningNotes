通过局部变量的析构函数来确保资源释放，根据是否正常退出来确定是否释放资源
```cpp
template<typename Func>
class ResGuard
{
public:
    explicit ResGuard(Func&& func) 
    : m_func(std::move(func))
    , m_ignore(false)
    {}

    explicit ResGuard(const Func& func)
    : m_func(func)
    , m_ignore(false)
    {}

    ~ResGuard() {
        if(!m_ignore && m_func != nullptr) {
            m_func();
        }
    } 

    ResGuard(ResGuard&& other)
    : m_func(std::move(other.m_func))
    , m_ignore(other.m_ignore)
    { other.ignore(); }

    void ignore() {
        m_ignore = true;
    }

private:
    Func m_func;
    bool m_ignore;

    ResGuard() ;
    ResGuard(const ResGuard&);
    ResGuard& operator=(const ResGuard&);
};

template<typename Func>
ResGuard<std::decay_t<Func>> makeGuard(Func&& func) {
    return ResGuard<std::decay_t<Func>>(std::forward<Func>(func));
}
```
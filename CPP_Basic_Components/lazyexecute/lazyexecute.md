惰性求值的使用场景：某些大对象并不需要立刻初始化，或者某些函数不需要立即执行，等到需要的时候在进行。这里可以结合之前的Optional
```cpp

template<typename T>
struct Lazy
{
    template<typename F, typoename...Args>
    Lazy(F&& func, Args&&...args) 
    {
        m_func = [&func, &args]{ return  func(args...); };
    }

    T& value() {
        if(!m_value.isInit()) {
            m_value = m_func();
        }
        return *m_value;
    }

    bool isValueGet() const {
        return m_value.isInit();
    }

private: 
    std::function<T()> m_func;
    Optional<T> m_value;
};

//全局函数，result_of从17开始，也可以使用std::invoke
template<typename Func, typename...Args, typename RT =  typename std::result_of<Func(Args...)>::type>
RT lazy(Func&& func, Args&&...args) 
{
    return Lazy<RT>(std::forward<Func>(func), std::forward<Args>(args)...);
}

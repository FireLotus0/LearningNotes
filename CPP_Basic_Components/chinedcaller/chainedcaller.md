链式调用：将多个函数按照上一个的输出作为下一个的输入，依次执行
```cpp
#include <functional>
#include <type_traits>

template<typename T>
class Task;

template<typename Func, typename...Args>
class Task<Func(Args...)>   //模版特化，模版参数是函数签名
{
private:
    std::function<Func(Args...)> m_func;

public:
    Task(std::function<Func(Args...)>&& func) : m_func(std::move(func)) {}

    Task(std::function<Func(Args...)>& func) : m_func(func) {}

    Func run(Args&&...args) {
        return m_func(std::forward<Args>(args)...);
    }

    template<typename F, typename RT = typename std::result_of<F(Func)>::type>
    RT then(F&& f)
    {
        auto func = std::move(m_func);
        return Task<RT(Args...)>([func, &f](Args&&...args){
            return f(func(std::forward<Args>(args)...));
        }) ;
    }
};
```
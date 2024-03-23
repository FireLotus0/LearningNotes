AOP: 面向方面编程。不同的业务的核心逻辑不同，但有一些通用的逻辑，每个业务中都存在权限验证，日志，事务处理等非核心逻辑的需求，AOP就是把这些重复的横切出来。业务具有核心关注点和横切关注点，横切关注点的特点就是在多个组件中重复出现，核心关注点则只在当前业务中出现。AOP的实现通常是在运行期间拦截方法，使用代理模式。
```cpp
/*代理模式示例*/

class Base
{
public:
    Base() {}

    virtual ~Base() {}

    virtual void func()  {}
};

class Derived : public Base
{
public:
    void func() override
    {}
};

class BaseProxy : public Base
{
private:
    Base* m_ptr;
public:
    BaseProxy(Base* bptr) : m_ptr(b_ptr) {}

    ~BaseProxy() {
        delete m_ptr;
        m_ptr = nullptr;
    }

    void func() final
    {
        std::cout << "Before real func: "<<std::endl;
        m_ptr->func();
        std::cout<< "After real func: "<<std::endl;
    }
};

void testProxy()
{
    std::shared_ptr<Base> derived = std::make_shared<BaseProxy>(new Derived);
    derived->func();
}

```
上面虽然可以通过代理实现对一个横切面的实现，但是存在无法组合多个横切面，且（必须依赖真实对象，将derived的指针传递给proxy），必须从基类继承实现其接口。解决办法：将切面作为模版组合，从而使用变参模版可以组合多个切面
```cpp

#define HAS_MEMBER(member) \
template<typename T, typename...Args> \ 
struct has_member_##member \ 
{ \
private: \
    template<typename U> \
    static auto check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type()); \
    template<typename U> \
    static std::false_type check(...); \
public: \
    enum { value = std::is_same_v<decltype(check<T>(0)), std::true_type>}; \
};

HAS_MEMBER(Before)
HAS_MEMBER(After)

template<typename Func, typename...Args>
struct Aspect
{
private:
    Func m_func;

public:
    Aspect(const Aspect&) = delete;
    Aspect& opertor=(const Aspect&) = delete;

public:
    Aspect(Func&& func) : m_func(std::forward<Func>(func)) {}

    template<typename T>
    std::enable_if_t<has_member_Before<T, Args>::value && has_member_After<T, Args...>::vlaue> Invoke(Args&& args, T&& aspect) 
    {
        //核心逻辑之前的切面逻辑
        aspect.Before(std::forward<Args>(args)...);
        //核心逻辑
        m_func(std::forward<Args>(args)...);    
        //核心逻辑之后的切面逻辑
        aspect.After(std::forward<Args>(args)...);
    }

    template<typename T>
    std::enable_if_t<has_member_Before<T, Args>::value && !has_member_After<T, Args...>::vlaue>  Invoke(Args&& args, T&& aspect)
    {
        //核心逻辑之前的切面逻辑
        aspect.Before(std::forward<Args>(args)...);
        //核心逻辑
        m_func(std::forward<Args>(args)...);  
    }

    template<typename T>
    std::enable_if_t<!has_member_Before<T, Args>::value && has_member_After<T, Args...>::vlaue>  Invoke(Args&& args, T&& aspect)
    {
        //核心逻辑
        m_func(std::forward<Args>(args)...);  
        //核心逻辑之后的切面逻辑
        aspect.After(std::forward<Args>(args)...);
    }

    template<typename Head, typename...Tail>
    void Invoke(Args&&...args, Head&& headAspect, Tail&&...tailAspect)
    {
        headAspect.Before(std::forward<Args>(args)...);
        Invoke(std::forward<Args>(args)..., std::forward<Tail>(tailAspect)...);
        headAspect.After(std::forward<Args>(args)...);
    }
};

template<typename T>
using identity_t = T;

//AOP的辅助函数，简化调用
template<typename...AP, typename...Args, typename Func>
void Invoke(Func&& func, Args&&...args)
{
    Aspect<Func, Args...> asp(std::forward<Func>(func));
    asp.Invoke(std::forward<Args>(args)..., identity_t<AP>()...);
}

/**test code
struct TimeElapseAspect
{
    void Before(int i)
    {
        m_lasttime = m_t.elapsed();
    }

    void After(int i)
    {
        std::cout << "time elapsed: " << m_t.elapsed() - m_lasttime << std::endl;
    }

private:
    double m_lasttime;
    Timer m_t;
};

struct LogginAspect
{
    void Before(int i)
    {
        std::cout << "entering " << std::endl;
    }

    void After(int i)
    {
        std::cout << "leaving" << std::endl;
    }
};

void foo(int a)
{
    std::cout << "real HT function: " << std::endl;
}

int main()
{
    Invoke<LogginAspect, TimeElapseAspect>(&foo, 1);
    std::cout <<"--------------------------------"<<std::endl;
    Invoke<TimeElapseAspect, LogginAspect>(&foo, 1);
}

*/

```
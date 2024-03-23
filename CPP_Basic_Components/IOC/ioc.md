IOC(Inversion of Control)：控制反转。应用本身不负责依赖对象的创建，而交给一个外部容器来负责，控制权转移到外部容器，即控制反转。IOC用来消除对象直接依赖产生的耦合。具体是将对象之间的依赖关系移出去，放到一个统一的XML配置文件中或者在IOC容器中配置。IOC容器具备两种能力：一种是对象工厂能力，可以创建所有类型的对象，可以进行配置；另一种是创建依赖对象，应用不需要直接创建依赖对象，由IOC容器创建，实现控制反转。实现IOC容器需要解决三个问题：
* 可以创建所有类型的对象
* 类型擦除
* 创建依赖对象

实现根据配置创建对象的思路：先注册可能需要创建的对象类型的函数，将其放到一个内部关联的容器中，设置键为类型的名称或者某个唯一的标识；值为对象类型的构造函数，创建时根据键查询对应的构造函数并创建，这个唯一标识可以写入到构造函数中去。如果需要实现能够创建所有类型的对象，需要通过类型擦除技术。  
  
类型擦除：
* 通过多态擦除类型  
* 通过模版擦除类型  
* 通过某种类型容器进行擦除  
* 通过某种通用类型擦除（QVariant）  
* 通过闭包进行擦除

多态擦除类型：将派生类通过隐式转换为基类，在通过基类去调用虚函数。这种方式缺点是基类类型必须存在，必须通过继承（强耦合）。  
模版则是将不同类型的公共行为提取出来，不同类型不需要通过继承获得公共行为，消除了不同类型之间的耦合，但是仍然必须要指定类型来实例化模版。  
通过类型容器来进行擦除，缺点是必须事先定义好所有的类型  
Any结合函数闭包，Any的缺点是取值的时候必须指明类型信息，而函数闭包可以将类型信息保存封装起来

```cpp
/*通过Any擦除类型改进对象工厂*/
class IoncContainer
{
public:
    IoncContainer() {}

    ~IocContainer() {}

    template<typename T, typename Depend>
    void registerType(const string& key)
    {
        //通过闭包擦除参数类型
        std::function<T*()> func = []{ return new T(new Depend()) };
        registerType(key, func);
    }

    template<typenmae T>
    T* resolve(const string& key)
    {
        if(m_creatorMap.find(key) == m_creatorMap.end())
        {
            return nullptr;
        }
        Any res = m_creatorMap[key];
        auto func = res.anyCast<std::function<T*()>>();
        return func();
    }

    template<typenmae T>
    shared_ptr<T> resolveShared(const string& key)
    {
        T* ptr = resolve<T>(key);
        return std::shared_ptr<T>(ptr);
    }
private:
    void registerType(const string& key, Any constructor)
    {
        if(m_creatorMap.find(key) != m_creatorMap.end())
        {
            throw std::invalid_arguments("already exsists!");
        }
        m_creatorMap.emplace(key, constructor);
    }

private:
    unordered_map<string, Any> m_creatorMap;
};
```

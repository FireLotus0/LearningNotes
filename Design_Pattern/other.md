* 派生类继承基类时将自己作为基类的模版参数传递给基类：例如，std::enable_shared_from_this<T>

* 混合继承：派生类继承于模版参数

* DOLID原则：
    * S: Single Responsibility Principle
    * O：Open Closed Principle
    * L: Liskov Subtitution Principle
    * I: Interface Segregation Principle
    * D: Dependency Inversion Principle

* 单一职责原则：
```cpp
/*场景： 写一个日记功能的类*/
struct Journal
{
    string title;
    vector<string> entries;
    explicit Journal(const string& title): title(title) {}

    void add(const string& entey)
    {
        static int count = 1;
        entries.push_back(std::to_string(count++) + entry);
    }
};
/*将entry添加到entries是Journal类的职责，现在假设需要进行持久化操作*/
void Journal::save(const striung& filename)
{
    ofstream of(filename);
    for(const auto& entry : entries) {
        of << entey << endl;
    }
}
/*这个方法存在问题：Journal的职责是持有entries，而不是将entries写回磁盘。如果在继承结构中，牵一发动全身，必须在其他相关类中相应的做出微小改变，这是不应该的。这里的重点是持久化功能是一个单独的功能，应当单独提取出来*/
struct PersistenceManager
{
    static void save(const Journal& j, const string& filename)
    {
        ofstream of(filename);
        for(const auto& s : j.entries)  {
            of << entry << endl;
        }
    }
};
/*单一职责原则：任何一个类有唯一的职责，除非职责改变，否则不应当修改*/
```

```cpp
/*Open-Closed Principle*/
/*场景：假设有一系列产品，每个产品都有颜色和大小的属性。现在需要一个过滤器对一系列产品进行过滤(标准库里面有std::logic_and....结合Functor)*/
enum class Color{ Red, Green, Blue};
enum class Size{ Big, Mid, Small};

struct Product
{
    string name;
    Color color;
    Size size;
};

struct ProductFilter
{
    using Items = vector<Product*>;

    Items by_color(Item items, Color color) {
        Items res;
        for(auto& i : items) {
            if(i->color == color) {
                res.push_back(i);
            }
        }
        return res;
    }
};
/*现在的问题是，很有可能还需要根据size进行过滤，又或者要根据颜色和状态一起进行过滤，这就是会发生变化的地方，这将会导致重复代码。为什么不让过滤器接受一个过滤条件。结合SRP原则，将过滤条件单独提取出来，将过滤函数单独提取出来*/
template<typename T>
struct Specification
{
    virtual bool is_satisfied(T* item) = 0;
};

template<typename T>
struct Filters
{
    virtual vector<T*> filter(vector<T*> items, Specification<T>& condition) = 0;
};

struct BetterFilter : Filter<Product>
{
    vector<Product*> filter(vector<Product*>& items, Specification<Product>& spec) override {
        vector<Product*> res;
        for(auto& i : items)
        {
            if(spec.is_satified(i)) {
                res.push_back(i);
            }
        }
        return res;
    }
}

//过滤条件
struct ColorSpecification : Spicification<Product> {
    Color color;
    explicit ColorSpicification(Color color) : color(color) {}

    bool is_satisfied(Product* product) override {
        return color == product->color;
    }
};

struct SizeSpicification : Spicification<Product>
{
    Size size;
    explicit SizeSpicification(Size size) : size(size) {}

    bool is_satisfied(Product* product) override {
        return size == product->size;
    }
};

//使用条件
BetterFilter betterFilter;
ColorSpicification colorSpec(Color::Red);
SizeSpicification sizeSpec(Size::Big);
vector<Product*> items;
auto clorRes = betterFilter.filter(items, colorSpec);
auot sizeRes = betterFilter.filter(items, sizeSpec);

struct SizeColorSpec : Specification<Proiduct>
{
    Color color;
    Size size;
    SizeColorSpec(Color color, Size size) : color(color), size(size) {}

    bool is_satisfied(Product* product) override
    {
        return size == product->size && color == product->color; 
    }
}
/*总结：可以看出开闭原则的特点，将变化分离出来，当发生变化时，可以增加处理变化的代码，而不用修改原来的代码。*/
```

```cpp
/*Interface Segragation Principle*/


/*场景：现在需要定义一个抽象接口给别人用，别人只需实现相关的虚函数即可.问题是，有些人可能只想使用print函数，但是不得不实现其他的虚函数。所以，接口隔离原则就是你应当分裂接口，让别人可以选择实现*/
struct Printer
{
    void print(vector<Document*> docs) = 0;
    void fax(vector<Document*> docs) = 0;
    void scan(vector<Document*> docs) = 0;
};

//接口隔离
struct IPrinter
{
    virtual void print(vector<Document*>) = 0;
};

struct IScanner
{
    virtual void scan(vector<Document*>) = 0;
};

struct Machine : IPrinter, IScanner
{

};

```

```cpp
/*Dependency Inversion Principle*/
/*假设要实现日志输出组件，则该组件不能依赖于具体的输出类，如FileLogger, ConsoleLogger，而应该依赖于抽象接口ILogger。这里的高层模块就是输出组件，它更接近于用户。抽象不应该依赖于具体实现，相反，具体实现依赖于抽象*/

class Reporting
{
    ILogger& logger;
public:
    Reporting(const ILogger& logger) : logger(logger) {

    }

    void report() {
        logger.log_info("");
    }
};
/*问题：现在Reporting的构建必须依赖于传入一个FileLogger或者ConsoleLogger....，也就是Reporting对象的构建依赖于ILogger对象，如果ILogger对象的构建又依赖于其他对象，这样循环下去怎么办，*/

struct Engine
{
    float volumn;
    friend ostream& operator<< (ostream& os, const Engine& engine){

    }
};

struct Logger
{
    ~Logger() {}
    virtual void Log(const string& str) = 0;
};

struct ConsoleLogger : Logger
{
    void Log(const string& ) override {
        
    }
};
```
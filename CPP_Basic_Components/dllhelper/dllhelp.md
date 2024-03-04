dll调用：加载dll后，需要定义一个对应的函数指针类型，然后调用GetProcAddress获取函数地址，再转换成指针进行调用。获取函数地址和调用的过程是重复的，应当消除，通过传入函数名称和对应参数就能够实现调用。思路是将函数指针转换成函数对象，通过函数名称获取一个function

```cpp
//函数所在头文件位置
#include <dlfcn.h>

/*dlopen以指定模式打开指定的动态连接库文件，并返回一个句柄给调用进程,该句柄为 ELF 对象的句柄。filename为文件所在目录，若非系统特定目录的动态库，则必须带上目录路径，否则运行时会找不到动态库。TLD_LAZY表示使用延迟绑定，当函数第一次被用到时才进行绑定。有助于提高加载动态库的速度。
RTLD_NOW表示当模块被加载时即完成所有函数的绑定工作，如果有任何未定义的符号引用的绑定工作没法完成，那么就返回错误。调试程序的时候可以选择这个选项。
RTLD_GLOBAL可以和上面两者中任意一个一起使用（或操作），表示将被加载的模块的全局符号合并到进程的全局符号表中，使以后加载的模块可以使用这些符号。
*/
void *dlopen(const char *filename, int flag);

/*通过调用 dlsym 来识别这个handle句柄对象内的符号的地址了。可以返回动态库中的函数或全局变量。symbol为想要调用的函数名称或变量名。*/
void *dlsym(void *handle, const char *symbol);

/*如果调用dlopen、dlsym API 时发生了错误，dlerror 函数返回一个字符串，在没有错误发生时返回 NULL。*/
char *dlerror(void);

/*来通知操作系统不再需要句柄和对象引用了，任何通过已关闭的对象的 dlsym 解析的符号都将不再可用。*/
int dlclose(void *handle);
```

```cpp
#include <map>
#include <functional>
#include <map>
#include <string>
#include <dlfcn.h>

class DllHelper
{
public:
    DllHelper() : handle{nullptr} {}

    ~DllHelper() {
        unLoad();
    }

    bool load(const std::string& dllPath, int flag) {
        handle = dlopen(dllPath.c_str(), flag);
        if(handle == nullptr) {
            std::cout << dlerror() std::endl;
            return false;
        }
        return true;
    }

    bool unLoad() {
        if(handle == nullptr) {
            return true;
        }
        dlclose(handle);
    }

    template<typename T>
    std::function<T> getFunctor(const std::string& funcName) {
        auto it = mp.find(funcName);
        if(it == mp.end()) {
            auto addr = dlsym(handle, funcName.c_str());
            if(addr == nullptr) {
                return nullptr;
            }
            mp.insert(std::make_pair(funcName, addr));
        }
        return std::function<T>((T*)addr);
    }

    template<typename T, typename...Args, typename RT = std::result_of<std::function<T>(Args...)>::type>
    RT execute(const std::string& funcName, ARgs&&...args) {
        auto functor = getFunctor<T>(funcName);
        if(functor == nullptr) {
            std::string res = "error: No such function find with name " + funcName;
            throw logic_error(res);
        }
        return functor(std::forward<T>(args)...);
    }

private:
    void* handle;
    std::map<std::string, void*> mp;
};
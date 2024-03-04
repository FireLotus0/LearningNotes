optional用于存储一个元素，实现未初始化的概念，可以用于解决函数返回无效值的问题，有时需要根据条件去查找某个值，如果没有找到，返回一个无效值，即返回一个未初始化的optional对象
```cpp
#include <type_traits>
#include <utility>
#include <stdexcept>

template<typename T>
class Optional
{
    /*alignedT: 内存对齐的对象类型*/
    using alignedT = typename std::aligned_storage<sizeof(T), std::alignof(T)::value>::type;

private:
    bool isInit;
    alignedT data;

private:
    /**
     * @brief 创建内存对齐的对象
     */
     template<typename...Args>
     void createObj(Args&&...args) {
        new (&data) T(std::forward<Args>(args)...);
        isInit = true;
     }

     /**
      * @brief 销毁对象，释放缓冲区
      */
      void destroyObj() {
        if(isInit) {
            isInit = false;
            (T*)(&data) -> ~T();
        }
      }

      /**
       * @brief 拷贝赋值
       */
       void copy(const Optional& other) {
            if(other.isInit) {
                destroy();  // 先释放自己
                new (&data) T(*((T*)(other.data)));
                isInit = true;
            } else {
                destroyObj();   // 拷贝未初始化的对象
            }
       }

       /**
        * @brief 移动赋值
        */
        void move(Optional&& other) {
            if(other.isInit) {
                destroyObj();
                new (&data) T(std::move(*((T*)(&other.data))));
                other.destroy(); /*调用T类型的移动构造，other的T对象持有的资源已经转移，不用担心other释放*/
                isInit = true;
            }
        }

public:
    Optional() : isInit{false} {}

    Optional(const Optional& other) {
        copy(other);
    }

    Optional(Optional&& other) {
        move(std::move(other));
    }

    Optional(const T& val) {
        createObj(val);
    }

    Optional(T&& val) {
        createObj(std::move(val));
    }

    Optional& operator=(const Optional& other) {
        copy(other);
        return *this;
    }

    Optional& operator=(Optional&& other) {
        move(std::move(other));
        return *this;
    }

    template<typename...Args>
    void emplace(Args&&...args) {
        destroyObj();
        createObj(std::forward<Args>(args)...);
    }

    bool isInit() const {
        return isInit;
    }

    explicit operator bool() const {
        return isInit();
    }

    T& operator*() {
        if(isInit) {
            return *((T*)(&data));
        }

        throw std::logic_error{"try to get data min an Optional which is not initialized!"};
    }

    const T& operator*() const {
        if(isInit) {
            return *((T*)(&data));
        }

        throw std::logic_error("try to get data in an Optional which is not initialized!");
    }

    T* operator->() {
        return &operator*();
    }

    const T* operator->() const {
        return &operator*();
    }

    bool operator==(const Optional& other) {
        return (*bool(*this)) != (!other) ? false : (!bool(*this) ? true : *(*this) == (*other));
    }

    bool operator<(const Optional<T>& other) const {
        return !other ? false : (!bool(*this) ? true : (*(*this) < *other));
    }

    bool operator!=(const Optional<T>& other) const {
        return !(*this == other);
    }
};
```
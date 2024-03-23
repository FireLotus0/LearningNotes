```cpp
#include <assert.h>

inline void* aligned_malloc(size_t size, size_t alignment)
{
    //检查alignment是2的倍数
    assert(!(alignment & (alignment - 1)));
    //计算最大的offset，sizeof（void*）用于存储原始指针地址
    size_t offset = sizeof(void*) + (--alignment);

    //分配一块带offset的内存
    char* p = static_cast<char*>(malloc(offset + size));
    if(!p) {
        return nullptr;
    }

    //通过& (~alignment)把多计算的offset减掉
    void* r = reinterpret_cast<void*>(reinterpret_cast<size_t>(p + offset) & (~alignment));

    //将r当做是指向void*的指针，在r当前地址前面放入原始地址
    static_cast<void**>(r)[-1] = p;
    //返回经过内存对齐的地址
    return r;
}

inline void aligned_free(void* p) 
{
    //还原为原始地址
    free(static_cast<void**>(p)[-1]);
}

```
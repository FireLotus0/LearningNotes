#### CAS原子操作
- 操作逻辑：首先将*this与expected进行按位比较，如果相等(success)，则将desired的值写入到/*this中(read-modify-write)，
	如果不相等(failure)，则将*this写入到(expected)中(read)，括号中的读写操作针对this原子变量  
- compare_exchange_weak和comppare_exchange_strong，各自有4个重载函数  
```c
// weak--------------------------------------
template<typename T>
bool compare_exchang_weak(T& expected, T desired, std::memory_order success, std::memory_order failure) noexcept;

template<typename  T>
bool compare_exchange_weak(T& expected, T desired, std::memory_order success, std::memory_order failure) volatile noexcept;

template<typename T>
bool compare_exchange_weak(T& expected, T desired, std::memory_order order = std::meomory_order_seq_cst)  noexcept;

template<typename T>
bool compare_exchange_weak(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) volatile noexcept;

// stron--------------------------------------
template<typename T>
bool compare_exchange_strong(T& expected, T desired, std::memory_order success, std::memory_order failure) noexcept;

template<typename T>
bool compare_exchange_strong(T& expected, T desired,  std::memory_order  success, std::memory_order failure) volatile noexcept;

template<typename T>
bool compare_exchange_strong(T& expected, T desired, std::memory_order order  =  std::memory_order_seq_cst)  noexcept;

template<typename T>
bool  compare_exchange_strong(T& expected, T desired, std::memory_order order  =  std::memory_order_seq_cst)  volatile noexcept;

```
- 为什么会有weak和strong的区别：因为有些硬件平台不支持在一个原子操作中完成比较和交换操作，即不支持CAS原子指令    
- success和failure的内存序：如果传入了success和failure的memory_order，则按传入的内存序执行。如果只传入了一个memory_order，则成功时的read-modify-write使用传入的内存序，失败时的内存序根据传入的memory_order决定，对应关系：std::memory_order_acquire----std::memory_order_acq_rel，std::memory_order_relaxed----std::memory_order_release
- 注意：
	-  ***如果failure对应的内存序比success对应的内存序更严格，或者failure对应的内存序时memory_order_acq_rel或memory_order_release，则行为未定义***    
	- 类似原型模式，***比较和拷贝都是按位进行的，拷贝构造函数，拷贝赋值函数，比较运算符重载不会调用***
- ***返回值***：true，原子变量的值成功被修改，false原子变量值未被修改

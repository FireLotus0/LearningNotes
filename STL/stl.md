* 迭代器适配器：对标准迭代器的扩展（输入迭代器，输出迭代器，正向迭代器，双向迭代器，随机迭代器），定义了插入迭代器，反向迭代器，移动迭代器。  


| 迭代器适配器       | 价功能格 |
|------------|:------------ | 
|反向迭代器   |以创建出双向迭代器和随机迭代器的反向版本。容器的成员函数rbegin()和rend()或者全局函数rbegin()和rend() |
|插入迭代器   |后向插入迭代器：需要容器具有push_back函数，std::back_inserter(container)获取后向插入迭代器<br>前向插入得带起：需要容器具有push_front函数，std::front_inserter(container)获取前向插入迭代器;<br>随机插入迭代器：需要容器具有insert函数，std::inserter(container, pos)获取随机插入迭代器  |  
|移动迭代器   |指向一段范围的元素，用于移动元素，避免拷贝。make_move_iterator,参数为一个迭代器，返回值是std::move_iterator |  

* std::for_each的用法:
```cpp
template< class InputIt, class UnaryFunc >
UnaryFunc for_each( InputIt first, InputIt last, UnaryFunc f );     // constexpr since C++20

template< class ExecutionPolicy, class ForwardIt, class UnaryFunc >
void for_each( ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryFunc f );  //since C++17

/*
* 对指定范围的元素分别解引用并传递进f中处理，如果传递了policy，则可能是乱序处理。如果f不支持移动，结果将是未定义的(c++11)，如果f具有返回值，则该返回值将会被忽略，但是在20版本中，将f作为返回值了，意味着如果我传递的是一个重载了函数调用运算符的struct，那么我可以获取一些特殊效果，嗯...,因为struct可以携带数据。
* 乱序处理的前提是满足：std::is_execution_policy_v<std::remove_cvref_t<ExecutionPolicy>> is true
* 和其他并行处理算法不同的是，for_each不会进行元素的拷贝，仅仅通过解引用迭代器进行访问
*/
```  

* 迭代器上的运算：  

| 函数        |      功能  |
|:-------------|:-----------|
|advance(iter, distance) | 将iter移动distance距离，diatance可以是正数或者负数 |
|disance(iter_1, iter_2) | 返回两个迭代器之间的元素个数 |
|next(iter, n) | 返回iter正向偏移n得到的迭代器 |
|prev(iter, n) | 与next()相反 |

* 智能指针：只能指向堆上创建的对象，因为在栈上创建的对象当栈帧释放，就已经自动释放了。
```cpp
//智能指针的创建
auto pA = std::make_shared<A>();    //传入A的构造函数参数
std::shared_ptr<A> pAA{ new A()};   // unique_ptr同样

//指向数组的智能指针

```
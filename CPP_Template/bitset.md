#### std::bitset<N>:固定大小的N比特序列，可以使用逻辑运算符进行操作，可以与整数和字符串进行转换。比特序列最右侧具有最低索引序号，和整数的二进制表示一样。bitset的所有成员函数都是constexpr修饰的，可以在常量表达式中使用bitset。

#### std::bitset<N>::reference：嵌套于std::bitset中的代理类。外面通过代理类访问bits中的某个bit，可读写且会直接访问底层数据。



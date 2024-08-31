#### 位域：使用bit声明的具有显式大小的类数据成员。邻近的比特可能会被组合在一起，共享字节。
- 声明方式：
```cpp
1. identifier(optional) attr(optional) size
2. identifier(optional) attr(optional) size brace-or-equal-initializer

attr - 任意数量的属性序列
identifier - bid-field名称。未命名则该位域表示填充的bit，未命名不能有cv限定。
size - 大于等于0。只有未命名的bit-field才可以等于0，此时具有特殊有意义。
brace-or-equal-initializer:默认成员初始化器，用于初始化bit-field
```

- 解释:
	1. bit-field的类型必须是整形或者枚举
	2. bit-field不能是static修饰的静态数据成员
	3. 没有纯右值的bit-field，左值向左值转换时，底层产生一个bit-field对象。
	4. bit-field占据的bit数是有限制的

 - 注意：
 	1. bit-field不一定会从一个字节的起始处开始，因此不能对bit-field取地址，即指针和non-const引用不允许
	2. const引用一个bit-field对象，则会创建一个类型为bit-field的临时对象，进行拷贝构造，并且引用绑定到临时对象

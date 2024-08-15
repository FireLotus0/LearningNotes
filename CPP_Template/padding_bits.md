#### 位填充：在学习原子操作中的compare_exchange_weak和compare_exchange_strong函数时，提到了padding bits，在此记录。
- 对象和值得解释:
	- 对象：一块数据存储区
	- 值：存储区中数据的意义和解释
- 对象的特征：
	- 类型
	- 大小size：可以使用sizeof计算大小
	- 内存对齐：可以使用alignof计算内存对齐
	- (存储区域)生命周期：automatic，static，dynamic，thread-local
	- 具有确定的值value
- 不属于object的：
	- value
	- 引用
	- 函数
	- 枚举
	- 类型
	- 类模版的特化，函数模版的特化
	- 变参参数包
	- 命名空间
	- this指针

#### object被创建的几种方式：
- 定义(ODR)：
	- ***定义和声明要去分开***
	- 函数签名，没有函数体，就只是声明
	- 外部存储标识符extern，语言链接标识符extern  'C'，有这些标识符修饰但是没有进行初始化，都是声明
	- 类或结构体中没有inline修饰的静态数据成员，属于声明，需要类外定义，***有inline声明的，可以直接类中定义(since  c++17)***
	- (deprecated)在class或struct中使用static constexpr修饰的数据成员，相当于已经定义了，此时，类外仅是声明
	- typedef属于声明，类型别名属于声明using S  = S1，using声明：using  T::data;


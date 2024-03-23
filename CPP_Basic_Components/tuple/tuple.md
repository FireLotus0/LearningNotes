* 打印tuple：由于tuple的元素是变参模版，外面不知道里面是什么数据，有时调试需要知道具体值，希望能打印出tuple中的元素.tuple不能通过for循环打印，也不能通过可变参数模版展开参数包的方式来展开。展开tuple的一种方法是通过模板类的特化和递归调用来展开，另一种是通过一个索引序列来展开  
```cpp
/*通过模板类的特化和递归调用展开*/
template<typename Tuple, std::size_t N>
struct TuplePrinter
{
    static void print(const Tuple& t) {
        TuplePrinter<Tuple, N - 1>::print(t);
        std::cout << " , "<< std::get<N-1>(t);
    }
};

template<typename Tuple>
struct TuplePrinter<Tuple, 1>
{
    static void print(const Tuple& t)
    {
        std::cout << std::get<0>(t);
    }
};

template<typename...Args>
struct PrintTuple(const std::tuple<Args...>& t)
{
    std::cout  << "(";
    TuplePrinter<dedcltype(t), sizeof...(Args)>::print(t);
    std::cout << ")";
};
```

```cpp
/*获取tuple的参数索引序列*/
/***************方法一************/
template<int...>
struct IndexTuple{};

template<int I, typename IndexTuple, typename...Types>
struct makeIndexesImlp;

template<int I, int...indexes, typename T, typename...Types>
struct makeIndexesImp<I, IndexTuple<Index...>, T, Types...>
{
    using type = typename makeIndexesImp<I + 1, IndexTuple<Index..., I>, Types...>::type;
};

template<int I, int...Indexes>
struct mkaeIndexesImp<I, IndexTuple<Indexes...>>
{
    using type = IndexTuple<Indexes...>;
};

template<typename...Types>
struct makeIndexes : makeIndexesImp<0, IndexTuple<>, Types...>
{};

/***************方法二************/
template<int I, typename IndexTuple, typename...Types>
struct makeIndexesReverseImp;

template<int I, int...Index, typename T, typename...Types>
struct makeIndexesReverseImp<I, IndexTuple<Indexes...>, T, Types...>
{
    using type = typename makeIndexesReverseImp<I - 1, IndexTuple<Indexes..., I - 1>, Types...>::type;
};

template<int I, int...Indexes>
struct makeIndexesReverseImp<I, IndexTuple<Indexes...>>
{
    using type = IndexTuple<Indexes...>;
};

template<typename...Types>
struct makeReverseIndexes : makeIndexesReverseImp<sizeof...(Types), IndexTuple<>, Types...>
{};

```


```cpp
/*打印tuple万能转换：将tuple转换成Args...*/
template<typename Last>
void printImp(Last&& last) {
    std::cout << " " << last << std::endl;
}

template<typename Head, typename...Tail>
void printImp(Head&& head, Tail&&...tail) {
    std::cout << " " << head;
    printImp(tail...);
}

template<typename...Args, int...Indexes>
void printTuple()
```
```cpp
template<typename T, int...Indexes>
void printImp(const T& tup, IndexTuple<Indexes...>)
{
    int a[] = { ((std::cout << std::get<Indexes>(tup) << " "), 0)...};

    std::cout << std::endl;
}

template<typename...Args>
void printTuple(const std::tuple<Args...>& tup)
{
    typedef typename makeIndexes<Args...>::type seqTyp;
    printImp(tup, seqType());
}
```
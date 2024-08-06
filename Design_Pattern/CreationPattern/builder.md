* 构建器模式关心的是创建复杂的对象,即不能在单行构造函数调用中构建的对象。这些类型的对象可能自己由其他对象组成,可能涉及不太明显的逻辑,因此需要单独的组件专门用于对象构建。
```cpp
class Person
{
    Person() {}

    string address, postCode, city;
    int id;
};

class PersonBuilderBase
{
protected:
    Person& person;
    explicit PersonBuilderBase(Person& person) : person(person) {}

public:
    operator Person() {
        return std::move(person);
    }

    PersonAddressBuilder lives() const;
    PersonJobBuilder works() const;
};

class PersonBuilder : public PersonBuilderBase
{
    Person person;
public:
    PersonBuilder() : PersonBuilderBase(person) {}
};

class PersonAddressBuilder : public PersonBuilderBase
{
    typedef PersonAddressBuilder self;
public:
    explicit PersonAddressBuilder(Person& person)
    : PersonBuilderBase(person) {}

    self& at(string address) {
        person.address = address;
        return *this;
    }

    self& postCode(string& code) {
        person.postCode = code;
        return *this;
    }
};

/*总结：构建器模式的目标是分段构建复杂的对象或组件*/
```
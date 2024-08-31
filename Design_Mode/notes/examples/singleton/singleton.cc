#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <atomic>

/****************************************
*单例模式：静态局部变量实现
****************************************/

// 懒汉式
template<typename T>
class Singleton_1
{
public:
	template<typename...Args>
	static T*  instance(Args&&...args) {
		static T* tPtr = new T(std::forward<Args>(args)...);
		return tPtr;
	}
	

public:
	Singleton_1() = delete;
	Singleton_1(const Singleton_1&) = delete;
	Singleton_1(Singleton_1&&) = delete;
	Singleton_1& operator=(const Singleton_1&) = delete;
	Singleton_1& operator=(Singleton_1&&) = delete;
};

// 饿汉式
//template<typename T, typename =  std::enable_if_t<std::is_default_constructible_v<T>>>
//class Singleton_2
//{
//public:
//	static T* instance() {
//		return tPtr;
//	}
//
//public:	
//	Singleton_2() = delete;
//	Singleton_2(const Singleton_2&) = delete;
//	Singleton_2(Singleton_2&&) = delete;
//	Singleton_2& operator=(const Singleton_2&) = delete;
//	Singleton_2& operator=(Singleton_2&&) = delete;
//
//private:
//	static T* tPtr;
//};
//
//template<typename T, typename = std::enable_if_t<std::is_default_constructible_v<T>>>
//T* Singleton_2<T>::tPtr = new T();

//静态成员变量，需要加锁
template<typename T>
class Singleton_3
{
public:
	//  不能使用，存在潜在的条件竞争问题
	template<typename...Args>
	static T* instance(Args&&...args) {
		if(ptr == nullptr) {
			std::lock_guard<std::mutex>  lock(mt);
			if(ptr == nullptr) {
				ptr = new T(std::forward<Args>(args)...);
			}
		}
		return ptr;
	}

public:
	Singleton_3() = default;
	Singleton_3(const Singleton_3&) = delete;
	Singleton_3(Singleton_3&&) = delete;
	Singleton_3& operator=(const Singleton_3&) = delete;
	Singleton_3& operator=(Singleton_3&&) = delete;

private:
	inline static T* ptr = nullptr;
	inline static std::mutex mt;
};


/****************************************
*单例模式：once_flag实现
****************************************/
template<typename T>
class Singleton_4
{

public:
	template<typename...Args>
	static void  create(Args&&...args) {
		tPtr = new T(std::forward<Args>(args)...);
	}
public:
	template<typename...Args>
	static T* instance(Args&&...args) {
		std::call_once(flag, &Singleton_4::create<Args...>, std::forward<Args>(args)...);
		return tPtr;
	}

public:	
	Singleton_4() = default;
	Singleton_4(const Singleton_4&) = delete;
	Singleton_4(Singleton_4&&) = delete;
	Singleton_4& operator=(const Singleton_4&) = delete;
	Singleton_4& operator=(Singleton_4&&) = delete;

private:
	inline static T* tPtr;
	inline static std::once_flag flag; 

};

/****************************************
* 原子操作
****************************************/
template<typename T>
class Singleton_5
{
public:
	template<typename...Args>
	static T* instance(Args&&...args)
	{
		auto tmpPtr = ptr.load(std::memory_order_acquire);
		if(tmpPtr != nullptr)  {
			return tmpPtr;
		}
		T* expected = nullptr;
		while(!ptr.compare_exchange_weak(expected, new T(std::forward<Args>(args)...), std::memory_order_acquire, std::memory_order_acq_rel));
		return ptr;
	}

public:
	Singleton_5() = delete;
	Singleton_5(const Singleton_5&) = delete;
	Singleton_5& operator=(const Singleton_5&) = delete;
	Singleton_5(Singleton_5&&) = delete;
	Singleton_5& operator=(Singleton_5&&)  = delete;

private:
	inline static std::atomic<T*> ptr  = nullptr;
};


/****************************************
* 测试
****************************************/
class TestWithParam
{
public:
	TestWithParam(const TestWithParam&) = delete;
	TestWithParam& operator=(const TestWithParam&)= delete;
	TestWithParam(TestWithParam&&) = delete;
	TestWithParam& operator=(TestWithParam&&) = delete;

public:
	TestWithParam(int data) {
		number = data;
	}

	void printNumber()  {
		std::lock_guard<std::mutex> lock(mt);
		std::cout << "thread id:" << std::this_thread::get_id() << "   number:"  << number  << " this:"  << this << std::endl;
	}

private:
	int number = 0;
	std::mutex mt;
};


void testFunc()
{
//	Singleton_1<TestWithParam>::instance(99)->printNumber();
//	Singleton_2<TestWithParam>::instance(99)->printNumber();
//	Singleton_3<TestWithParam>::instance(99)->printNumber();
//	Singleton_4<TestWithParam>::instance(99)->printNumber();
	Singleton_5<TestWithParam>::instance(99)->printNumber();
}


int main() 
{
	std::vector<std::thread> threads;
	for(int i = 0; i < 5; i++) {
		threads.emplace_back(std::thread(testFunc));
	}
	testFunc();
	for(auto& t : threads) {
		t.join();
	}

	return 0;
}


/****************************************
*单例模式：静态局部变量实现
****************************************/

// 懒汉式
template<typename T>
class Singleton
{

	template<typename...Args>
	static T*  instance(Args&&...args) {
		static T* tPtr = new T(std::forward<Args>(args)...);
		return tPtr;
	}
	

public:
	Singleton() = delete;
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;
};

// 饿汉式
template<typename T, typename =  std::enable_if_t<std::is_default_constructible_v<T>>>
class Singleton
{
public:
	static T* instance() {
		return tPtr;
	}

public:	
	Singleton() = delete;
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;

private:
	static T* tPtr;
};

template<typename T>
T* Singleton<T>::tPtr = new T();




/****************************************
*单例模式：once_flag实现
****************************************/
template<typename T>
class Singleton
{
public:
	template<typename...Args>
	static T* instance(Args&&...args) {
		std::call_once(flag, &Singleton::create,  std::forward<Args>(args)...);
		return tPtr;
	}

public:	
	Singleton() = delete;
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;

private:
	template<typename...Args>
	void  create(Args&&...args) {
		tPtr = new T(std::forward<Args>(args)...);
	}

private:
	static T* tPtr;
	static std::once_flag flag; 

};

template<typename T>
T* Singleton<T>::tPtr  = nullptr;

template<typename T>
std::once_flag Singleton<T>::flag;





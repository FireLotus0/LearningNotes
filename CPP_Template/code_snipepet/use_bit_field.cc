#include <iostream>
#include <bit>
#include <cstdint>

struct S
{
	unsigned int b : 3;		//只使用3个bit，只能存储0-7
};

struct C{
	unsigned char b1 : 3;	// 结构体第一个字节
	unsigned char : 2;		// 结构体第一个字节接着的两个bit不可使用了
	unsigned char b2 : 6;	// 结构体第一个字节剩下3个bit，所以b2占据第二个字节
	unsigned char b3 : 2;	// 占据第二个字节
	
};

void testC(){
	std::cout << "sizeof(C) is " << sizeof(C) << std::endl;

	C c;
	c.b1 = 0b111;
	c.b2 = 0b101111;
	c.b3 = 0x11;

	auto i = std::bit_cast<std::uint16_t>(c);

	for(auto b = i; b; b >>= 1) {
		std::cout << (b & 1);
	}
	std::cout << std::endl;

}

void testD() {
	struct D{
		unsigned char d1:3;
		unsigned char : 0;
		unsigned char d2:6;
	};

	std::cout << "sizeof(D) is " << sizeof(D) << std::endl;
}


int main() 
{
	S s{5};

	std::cout << "s.b = " << ++s.b << " sizeof(s) = "<< sizeof(s)<<std::endl;
	
	s.b = 8;
	std::cout << "after s.b = 8, the real value is " << s.b << std::endl;

	testC();

	testD();

	return 0;
}

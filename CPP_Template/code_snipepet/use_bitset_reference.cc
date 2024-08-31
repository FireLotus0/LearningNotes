#include <iostream>
#include <bitset>
#include <cassert>
#include <cstddef>


int main()
{
	std::cout << "*****************************************************************" << std::endl;
	std::bitset<4> bs{0b1110};

	std::bitset<4>::reference ref = bs[2];

	auto info = [&](int id){
		std::cout << id << ") bs:" << bs << "; ref bit: " << ref << std::endl;
	};

	info(1);
	ref = false;
	info(2);
	ref = true;
	info(3);
	ref.flip();
	info(4);
	ref = bs[1];
	info(5);

	std::cout << "6) ~ref bit: " << ~ref << std::endl;
	std::cout << "*****************************************************************" << std::endl;

	typedef std::size_t length_t, position_t;

	//构造函数
	constexpr std::bitset<4> b1;
	constexpr std::bitset<4> b2{0xA};	// 0b1010

	std::bitset<4> b3{"0011"};
	std::bitset<8> b4{"ABBA", length_t(4), /*0:*/'A', /*1:*/'B'};	// 0b0000 0110

	std::cout << "b1:" << b1 << "; b2:" << b2 << "; b3:" << b3 << "; b4:" << b4 << std::endl;

	//bitset supports bitwise operations
	b3 |= 0b0100; assert(b3 == 0b0100);
	b3 &= 0b0011; assert(b3 == 0b0011);
	b3 ^= std::bitset<4>{0b1100}; assert(b3 == 0b1111);

	// operations on the whole set
	b3.reset(); assert(b3 == 0);
	b3.set(); assert(b3 == 0b1111);
	assert(b3.all() && b3.any() && !b3.none());
	b3.flip(); assert(b3 == 0);

	//operations on individual bits
	b3.set(position_t(1), true); assert(b3 == 0b0010);
	b3.set(position_t(1), false); assert(b3 == 0);
	b3.flip(position_t(2)); assert(b3 == 0b0100);
	b3.reset(position_t(2)); assert(b3 == 0);

	//operator[]
	b3[2] = true;
	assert(true == b3[2]);

	// other operations
	assert(b3.count() == 1);
	assert(b3.size() == 4);
	assert(b3.to_ullong() == 0b0100ULL);
	assert(b3.to_string() == "0100");
}

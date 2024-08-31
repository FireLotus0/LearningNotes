#include <iostream>

struct S
{
  int member;
};

int main()
{
	S s1, s2;
	s1.member = 10;
	s2.member = 20;

	int S::* mp = &S::member;

	std::cout << "s1 member is " << s1.*mp << std::endl;
	std::cout << "s2 member is " << s2.*mp << std::endl;

	
	return 0;
}

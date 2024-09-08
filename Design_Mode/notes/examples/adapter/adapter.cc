#include <iostream>
#include <memory>
#include <string>

class ITargetInterface
{
public:
	virtual void play() = 0;
};

class MP3Player
{
public:
	void play() {
		std::cout << "play mp3 music..." << std::endl;
	}
};

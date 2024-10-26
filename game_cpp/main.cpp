#include "../framework/engine.hpp"

#include <random>
#include <ctime>


int main()
{
	std::srand(static_cast<uint32_t>(std::time(nullptr)));
	engine::run();
	return 0;
}

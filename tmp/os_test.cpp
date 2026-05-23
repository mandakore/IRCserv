#include <iostream>

int main () {
#ifdef __linux__
	std::cout << "Hello World!" << std::endl;

#elif defined(__APPLE__)
	std::cout << "This is Mac OS." << std::endl;
#endif
	return 0;
}
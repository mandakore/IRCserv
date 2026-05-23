#ifndef DEBUG_HPP
#define DEBUG_HPP

#define RESET "\033[0m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"

#define DEBUG(msg) std::cout << YELLOW << "[DEBUG]" << RESET << msg << std::endl

#endif

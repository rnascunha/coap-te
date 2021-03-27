#include "port.hpp"
#include <ctime>

namespace CoAP{

time_t time() noexcept
{
	return std::time(NULL);
}

unsigned random_generator() noexcept
{
	return std::rand();
}

}//CoAP

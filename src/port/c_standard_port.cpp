#include "port.hpp"

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

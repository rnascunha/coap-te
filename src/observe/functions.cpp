#include "functions.hpp"

namespace CoAP{
namespace Observe{

unsigned generate_option_value_from_clock() noexcept
{
	return CoAP::time() % max_option_value;
}

}//CoAP
}//Observe

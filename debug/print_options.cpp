#include "print_options.hpp"
#include <iostream>
#include "output_string.hpp"

namespace CoAP{
namespace Debug{

void print_option(CoAP::Message::option const* op)
{
	std::cout << "[" << static_cast<int>(op->code) << "] "
			<< CoAP::Debug::option_string(op->code) << " l: " << op->length;
}

void print_options(CoAP::Message::option const* op, std::size_t option_num)
{
	for(std::size_t i = 0; i < option_num; i++)
	{
		std::cout << i << ": ";
		print_option(&op[i]);
		std::cout << "\n";
	}
}

void print_options(CoAP::Message::option_node const* list)
{
	int c = 0;
	for(CoAP::Message::option_node const* i = list; i != nullptr; i = i->next)
	{
		std::cout << c++ << ": ";
		print_option(&i->value);
		std::cout << "\n";
	}
}

}//Debug
}//CoAP

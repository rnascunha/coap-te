#include "options.hpp"
#include <cstring>

namespace CoAP{
namespace Message{

option::option(){}
option::option(option_code code_, unsigned len, const void* val) :
	code(code_), length(len), value(val){}

option::option(option_code code)
{
	create_option<false>(*this, code);
}

option::option(option_code code, const char* value)
{
	create_option<false>(*this, code, value);
}

option::option(option_code code, unsigned& value)
{
	create_option<false>(*this, code, value);
}

option::option(option_code code, const void* value, unsigned length)
{
	create_option<false>(*this, code, value, length);
}

option_config const * get_option_config(option_code code) noexcept
{
	for(unsigned i = 0; i < (sizeof(options) / sizeof(option_config)); i++)
		if(code == options[i].code) return &options[i];

	return nullptr;
}

void exchange(option* first, option* second) noexcept
{
	option middle{option_code::invalid, 0, nullptr};

	std::memcpy(&middle, first, sizeof(option));
	std::memcpy(first, second, sizeof(option));
	std::memcpy(second, &middle, sizeof(option));
}

void exchange(option_node* first, option_node* second) noexcept
{
	option_node middle;

	std::memcpy(&middle.value, &first->value, sizeof(option));
	std::memcpy(&first->value, &second->value, sizeof(option));
	std::memcpy(&second->value, &middle.value, sizeof(option));
}

void sort_options(option* options, std::size_t num) noexcept
{
	option* option = nullptr;
	for(std::size_t i = 0; i < num; i++)
	{
		option = &options[i];
		std::size_t exchange_index = i;
		std::size_t j = i + 1;
		for(; j < num; j++)
			if(option->code > options[j].code)
				if(options[exchange_index].code > options[j].code)
					exchange_index = j;

		if(i != exchange_index)
			exchange(option, &options[exchange_index]);
	}
}

void sort_options(option_node* list) noexcept
{
	option* option = nullptr;
	for(option_node* i = list; i != nullptr; i = i->next)
	{
		option = &i->value;
		option_node* exchange_opt = i;
		option_node* j = i->next;
		for(; j != nullptr; j = j->next)
			if(option->code > j->value.code)
				if(exchange_opt->value.code > j->value.code)
					exchange_opt = j;

		if(i != exchange_opt)
			exchange(i, exchange_opt);
	}
}

}//Message
}//CoAP


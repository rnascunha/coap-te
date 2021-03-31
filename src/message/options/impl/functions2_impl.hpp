#ifndef COAP_TE_MESSAGE_OPTIONS_FUNCTIONS2_IMPL_HPP__
#define COAP_TE_MESSAGE_OPTIONS_FUNCTIONS2_IMPL_HPP__

#include "../functions2.hpp"
#include <cstring>
#include <type_traits>

#include "internal/helper.hpp"

#include "../../parser.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode>
std::size_t serialized_size(option_template<OptionCode> const& op,
		option_template<OptionCode> const& before) noexcept
{
	std::size_t size = 1;
	unsigned delta = before ?
								static_cast<unsigned>(op.ocode) - static_cast<unsigned>(before.ocode)
								: static_cast<unsigned>(op.ocode);

	size += delta > 12 ? (delta < 255 ? 1 : 2) : 0;
	size += op.length > 12 ? (op.length < 255 ? 1 : 2) : 0;

	return size + op.length;
}

template<typename OptionCode>
void exchange(option_template<OptionCode>* first, option_template<OptionCode>* second) noexcept
{
	option_template<OptionCode> middle{invalid<OptionCode>(), 0, nullptr};

	std::memcpy(&middle, first, sizeof(option_template<OptionCode>));
	std::memcpy(first, second, sizeof(option_template<OptionCode>));
	std::memcpy(second, &middle, sizeof(option_template<OptionCode>));
}

template<typename OptionCode>
void exchange(node_option<OptionCode>* first, node_option<OptionCode>* second) noexcept
{
	node_option<OptionCode> middle;

	std::memcpy(&middle.value, &first->value, sizeof(option_template<OptionCode>));
	std::memcpy(&first->value, &second->value, sizeof(option_template<OptionCode>));
	std::memcpy(&second->value, &middle.value, sizeof(option_template<OptionCode>));
}

template<typename OptionCode>
void sort(option_template<OptionCode>* optionss, std::size_t num) noexcept
{
	option_template<OptionCode>* option = nullptr;
	for(std::size_t i = 0; i < num; i++)
	{
		option = &optionss[i];
		std::size_t exchange_index = i;
		std::size_t j = i + 1;
		for(; j < num; j++)
			if(option->ocode > optionss[j].ocode)
				if(optionss[exchange_index].ocode > optionss[j].ocode)
					exchange_index = j;

		if(i != exchange_index)
			exchange<OptionCode>(option, &optionss[exchange_index]);
	}
}

template<typename OptionCode>
void sort(node_option<OptionCode>* list) noexcept
{
	option_template<OptionCode>* option = nullptr;
	for(node_option<OptionCode>* i = list; i != nullptr; i = i->next)
	{
		option = &i->value;
		node_option<OptionCode>* exchange_opt = i;
		node_option<OptionCode>* j = i->next;
		for(; j != nullptr; j = j->next)
			if(option->ocode > j->value.ocode)
				if(exchange_opt->value.ocode > j->value.ocode)
					exchange_opt = j;

		if(i != exchange_opt)
			exchange<OptionCode>(i, exchange_opt);
	}
}

template<typename OptionCode>
unsigned parse_unsigned(option_template<OptionCode> const& opt) noexcept
{
	unsigned value;
	CoAP::Helper::array_to_unsigned(static_cast<std::uint8_t const*>(opt.value), opt.length, value);

	return value;
}

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_FUNCTIONS2_IMPL_HPP__ */

#ifndef COAP_TE_MESSAGE_OPTIONS_FUNCTIONS2_HPP__
#define COAP_TE_MESSAGE_OPTIONS_FUNCTIONS2_HPP__

#include "template_class.hpp"
#include "../types.hpp"
#include "types.hpp"
#include "list.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode>
std::size_t serialized_size(option_template<OptionCode> const&,
							option_template<OptionCode> const& before) noexcept;

template<typename OptionCode>
void exchange(option_template<OptionCode>* first, option_template<OptionCode>* second) noexcept;
template<typename OptionCode>
void sort(option_template<OptionCode>* options, std::size_t num) noexcept;
template<typename OptionCode>
void sort(node_option<OptionCode>*) noexcept;

template<typename OptionCode>
unsigned parse_unsigned(option_template<OptionCode> const&) noexcept;

}//Option
}//Message
}//CoAP

#include "impl/functions2_impl.hpp"

#endif /* COAP_TE_MESSAGE_OPTIONS_FUNCTIONS2_HPP__ */

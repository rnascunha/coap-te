#ifndef COAP_TE_MESSAGE_OPTIONS_LIST_HPP__
#define COAP_TE_MESSAGE_OPTIONS_LIST_HPP__

#include "internal/list.hpp"
#include "template_class.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode>
using List_Option = CoAP::list<option_template<OptionCode>>;
template<typename OptionCode>
using node_option = typename List_Option<OptionCode>::node;

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_LIST_HPP__ */

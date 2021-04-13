#ifndef COAP_TE_OBSERVE_FUNCTIONS_HPP__
#define COAP_TE_OBSERVE_FUNCTIONS_HPP__

#include "types.hpp"

namespace CoAP{
namespace Observe{

template<typename Message>
message_status process_message(Message const&) noexcept;

}//CoAP
}//Observe

#include "impl/functions_impl.hpp"

#endif /* COAP_TE_OBSERVE_FUNCTIONS_HPP__ */

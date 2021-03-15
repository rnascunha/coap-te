#ifndef COAP_TE_RESOURCE_TYPES_HPP__
#define COAP_TE_RESOURCE_TYPES_HPP__

#include "transmission/response.hpp"
#include "message/types.hpp"
#include "internal/list.hpp"

namespace CoAP{
namespace Resource{

template<typename Endpoint>
using callback = void(CoAP::Message::message const&, CoAP::Transmission::Response<Endpoint>&);

}//Resource
}//CoAP

#endif /* COAP_TE_RESOURCE_TYPES_HPP__ */

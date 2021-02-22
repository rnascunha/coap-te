#ifndef COAP_TE_SECURITY_HPP__
#define COAP_TE_SECURITY_HPP__

#include <cstdint>

namespace CoAP{
namespace Security{

//https://tools.ietf.org/html/rfc7252#section-9
enum class mode {
	no_sec = 0,			//NoSec
	pre_shared_key,		//PreSharedKey
	raw_public_key,		//RawPublicKey
	certificate			//Certificate
};

static constexpr std::uint16_t default_port = 5684;

}//Security
}//CoAP

#endif /* COAP_TE_SECURITY_HPP__ */

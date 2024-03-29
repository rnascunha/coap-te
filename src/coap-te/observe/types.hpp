#ifndef COAP_TE_OBSERVE_TYPES_HPP__
#define COAP_TE_OBSERVE_TYPES_HPP__

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "../port/port.hpp"
#include "../message/types.hpp"
#include "../message/options/options.hpp"
#include "../message/options/parser.hpp"
#include "../message/options/functions2.hpp"

namespace CoAP{
namespace Observe{

static constexpr const unsigned reg = 0;
static constexpr const unsigned dereg = 1;

static constexpr const CoAP::Message::content_format
	invalid_format = static_cast<CoAP::Message::content_format>(255);

enum message_status{
	register_ = 0,
	deregister = 1,
	not_found,

};

static constexpr const unsigned max_option_value = 8388608; //2^23

struct order{
	unsigned 		value = 0;
	CoAP::time_t	time = 0;
};

}//CoAP
}//Observe

#endif /* COAP_TE_OBSERVE_TYPES_HPP__ */

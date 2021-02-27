#ifndef COAP_TE_TYPES_HPP__
#define COAP_TE_TYPES_HPP__

#include <cstdint>
#include "codes.hpp"

namespace CoAP{
namespace Message{

static constexpr const std::uint8_t version = 0b01;

enum class type : std::uint8_t
{
	confirmable 			= 0,
	nonconfirmable 			= 1,
	acknowledgement 		= 2,
	reset 					= 3
};

enum class content_format : unsigned
{
	text_plain				= 0,	//text/plain;charset=utf-8
	application_link_format = 40,	//application/link-format
	application_xml			= 41, 	//application/xml
	application_octet_stream = 42, 	//application/octet-stream
	application_exi			= 47, 	//application/exi
	application_json		= 50	//application/json
};

static constexpr const std::uint8_t payload_marker = 0xff;

struct message{
	type			mtype;
	code			mcode;
	uint16_t		mid;
	void const*		token;
	std::size_t		token_len = 0;
	std::uint8_t const* option_init;
	std::size_t		options_len;
	std::size_t		option_num = 0;
	void const*		payload;
	std::size_t		payload_len = 0;
};

bool check_type(type);

}//Message
}//CoAP

#endif /* COAP_TE_TYPES_HPP__ */

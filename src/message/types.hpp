#ifndef COAP_TE_MESSAGES_TYPES_HPP__
#define COAP_TE_MESSAGES_TYPES_HPP__

#include <cstdlib>
#include <cstdint>
#include "codes.hpp"

namespace CoAP{
namespace Message{

static constexpr const std::uint8_t version = 0b01;

enum class type : std::uint8_t
{
	confirmable 			= 0,
	nonconfirmable 			= 1,
	acknowledgment	 		= 2,
	reset 					= 3
};

enum class content_format : unsigned
{
	text_plain				= 0,	//text/plain;charset=utf-8
	application_link_format = 40,	//application/link-format
	application_xml			= 41, 	//application/xml
	application_octet_stream = 42, 	//application/octet-stream
	application_exi			= 47, 	//application/exi
	application_json		= 50,	//application/json
#if COAP_TE_FETCH_PATCH == 1
	//https://tools.ietf.org/html/rfc8132#section-6
	 application_json_patch_json = 51,	//application/json-patch+json
	 application_merge_patch_json = 52	//application/merge-patch+json
#endif /* COAP_TE_FETCH_PATCH == 1 */
};

using accept = content_format;

static constexpr const std::uint8_t payload_marker = 0xff;

struct message{
	type			mtype;				///< Message type (check enum class type)
	code			mcode;				///< Message code (check enum class code)
	uint16_t		mid;				///< Message ID
	void const*		token = nullptr;	///< Pointer to token
	std::size_t		token_len = 0;		///< Token length
	std::uint8_t const* option_init = nullptr;	///< Pointer to init of options
	std::size_t		options_len = 0;	///< Size at buffer of options
	std::size_t		option_num = 0;		///< Number of options
	void const*		payload = nullptr;	///< Pointer to payload init
	std::size_t		payload_len = 0;	///< Size of payload

	std::size_t size() const noexcept;
};

bool check_type(type);

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGES_TYPES_HPP__ */

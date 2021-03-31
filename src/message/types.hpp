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

#if COAP_TE_RELIABLE_CONNECTION == 1

enum class extend_length
{
	normal			= 0,
	one_byte 		= 13,
	two_bytes		= 14,
	three_bytes		= 15
};

struct message_reliable{
	unsigned		len = 0;
	std::size_t		token_len = 0;		///< Token length
	code			mcode;				///< Message code (check enum class code)
	void const*		token = nullptr;	///< Pointer to token
	std::uint8_t const* option_init = nullptr;	///< Pointer to init of options
	std::size_t		options_len = 0;	///< Size at buffer of options
	std::size_t		option_num = 0;		///< Number of options
	void const*		payload = nullptr;	///< Pointer to payload init
	std::size_t		payload_len = 0;	///< Size of payload

	std::size_t size() const noexcept;
};

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGES_TYPES_HPP__ */

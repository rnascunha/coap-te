#ifndef COAP_TE_MESSAGE_HPP__
#define COAP_TE_MESSAGE_HPP__

#include <cstdint>
#include "error.hpp"
#include "codes.hpp"
#include "options.hpp"

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

enum class content_format : std::uint8_t
{
	text_plain				= 0,	//text/plain;charset=utf-8
	application_link_format = 40,	//application/link-format
	application_xml			= 41, 	//application/xml
	application_octet_stream = 42, 	//application/octet-stream
	application_exi			= 47, 	//application/exi
	application_json		= 50	//application/json
};

struct header {
//	std::uint8_t	ver:2;
//	std::uint8_t	type:2;
//	std::uint8_t	token_len:4;
//	code			code;
//	uint16_t		message_id;
};

struct message {
	header			head;
	std::uint8_t	token[8];
	std::size_t		token_len;
	Option_List*	first;
	std::uint8_t*	payload;
	std::size_t		payload_len;
};

static constexpr const std::uint8_t payload_marker = 0xff;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* const token, std::size_t token_len,
		option* options, std::size_t option_num,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* const token, std::size_t token_len,
		option_node*,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* const token, std::size_t token_len,
		Option_List&,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

}//Message
}//CoAP

#include "impl/message_impl.hpp"

#endif /* COAP_TE_MESSAGE_HPP__ */


#ifndef COAP_TE_MESSAGES_TYPES_HPP__
#define COAP_TE_MESSAGES_TYPES_HPP__

#include <cstdlib>
#include <cstdint>
#include "codes.hpp"
#include "detail.hpp"

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

using content_format = detail::content_format;
using accept = detail::accept;

static constexpr const std::uint8_t payload_marker = 0xff;

struct message{
	message() = default;
	message(message const&) = default;

	type			mtype = type::confirmable;	///< Message type (check enum class type)
	code			mcode = code::empty;		///< Message code (check enum class code)
	uint16_t		mid = 0;					///< Message ID
	void const*		token = nullptr;	///< Pointer to token
	std::size_t		token_len = 0;		///< Token length
	std::uint8_t const* option_init = nullptr;	///< Pointer to init of options
	std::size_t		options_len = 0;	///< Size at buffer of options
	std::size_t		option_num = 0;		///< Number of options
	void const*		payload = nullptr;	///< Pointer to payload init
	std::size_t		payload_len = 0;	///< Size of payload

	std::size_t size() const noexcept;
	void clear() noexcept;
};

bool check_type(type);

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGES_TYPES_HPP__ */

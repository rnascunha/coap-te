#ifndef COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_IMPL_HPP__

#include "../functions.hpp"
#include "message/reliable/serialize.hpp"
#include "message/reliable/factory.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<bool SetLength>
std::size_t make_response_code_error(CoAP::Message::Reliable::message const& msg,
		std::uint8_t* buffer, std::size_t buffer_len,
		CoAP::Message::code err_code, const char* payload /* = "" */) noexcept
{
	CoAP::Error ec;
	std::uint8_t token[8];
	std::memcpy(token, msg.token, msg.token_len);

	return CoAP::Message::Reliable::serialize<SetLength>(
			buffer, buffer_len,
			err_code, token, msg.token_len,
			nullptr, payload, std::strlen(payload), ec);
}

template<bool SetLength>
std::size_t make_csm_message(csm_configure const& config,
		void* buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	using namespace CoAP::Message;

	CoAP::Message::Reliable::Factory<0, code::csm> fac{};

	unsigned max_size = config.max_message_size;
	Option::node_csm max_size_op{Option::csm::max_message_size, max_size};
	Option::node_csm block_wise_op{Option::csm::block_wise_transfer};

	fac.add_option(max_size_op);

	if(config.block_wise_transfer)
	{
		fac.add_option(block_wise_op);
	}

	return fac.serialize<SetLength, false, false, false>(static_cast<std::uint8_t*>(buffer), buffer_len, ec);
}

template<bool SetLength>
std::size_t make_abort_message(CoAP::Message::Option::option_abort& op,
		const char* payload,
		void* buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	using namespace CoAP::Message;

	Option::node_abort abort_op{op};
	CoAP::Message::Reliable::Factory<0, code::abort> fac{};
	if(op)
	{
		fac.add_option(abort_op);
	}

	if(payload && std::strlen(payload))
	{
		fac.payload(payload);
	}

	return fac.serialize<SetLength, false, false, false>(static_cast<std::uint8_t*>(buffer), buffer_len, ec);
}

/**
 * REMOVE THIS FUNCTION AS INLINE
 */
inline void process_signaling_csm(csm_configure& csm,
		CoAP::Message::Reliable::message const& msg) noexcept
{
	using namespace CoAP::Message;

	Option::Parser<Option::csm> parse(msg);
	Option::option_csm const* opt;
	while((opt = parse.next()))
	{
		switch(opt->ocode)
		{
			case Option::csm::max_message_size:
				csm.max_message_size = Option::parse_unsigned<Option::csm>(*opt);
				break;
			case Option::csm::block_wise_transfer:
				csm.block_wise_transfer = true;
				break;
			default:
				break;
		}
	}
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_IMPL_HPP__ */

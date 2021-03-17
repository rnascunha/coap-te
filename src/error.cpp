#include "error.hpp"

namespace CoAP{

int Error::value() const noexcept
{
	return err_;
}

const char* Error::message() const noexcept
{
	return Error::message(err_);
}

const char* Error::message(int error)
{
#ifndef COAP_TE_REMOVE_ERROR_MESSAGES
	switch(static_cast<errc>(error))
	{
		case errc::insufficient_buffer:	return "insufficient_buffer";
		case errc::invalid_token_length: return "invalid token length";
		case errc::message_too_small:	return "message too small";
		case errc::version_invalid:		return "invalid version";
		case errc::type_invalid:		return "invalid type";
		case errc::code_invalid: 		return "invalid code";
		case errc::empty_format_error:	return "empty format error";
		case errc::option_invalid: 		return "invalid option";
		case errc::option_out_of_order:	return "option out of order";
		case errc::option_repeated: 	return "option repeated";
		case errc::option_parse_error:	return "parse option error";
		case errc::payload_no_marker:	return "no payload marker";
		case errc::socket_error:		return "socket error";
		case errc::endpoint_error:		return "endpoint error";
		case errc::transaction_ocupied:	return "transaction ocupied";
		case errc::no_free_slots:		return "no transacition free slot";
		case errc::buffer_empty:		return "buffer empty";
		case errc::request_not_supported: return "request not supported";
		default:
			break;
	}
	return "(unrecognized error)";
#else
	return "";
#endif
}

Error::operator bool() const
{
	return err_ != 0;
}

}//CoAP

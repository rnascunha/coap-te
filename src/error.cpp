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
	switch(static_cast<errc>(error))
	{
		case errc::insufficient_buffer:	return "insufficient_buffer";
		case errc::invalid_token_length: return "invalid token length";
		case errc::message_too_small:	return "message too small";
		case errc::version_invalid:		return "invalid version";
		case errc::type_invalid:		return "invalid type";
		case errc::code_invalid: 		return "invalid code";
		case errc::option_invalid: 		return "invalid option";
		case errc::option_out_of_order:	return "option out of order";
		case errc::option_repeated: 	return "option repeated";
		case errc::option_parse_error:	return "parse option error";
		case errc::payload_no_marker:	return "no payload marker";
		case errc::socket_error:		return "socket error";
		case errc::endpoint_error:		return "endpoint error";
		case errc::no_free_slots:		return "no transacition free slot";
		default:
			break;
	}
	return "(unrecognized error)";
}

Error::operator bool() const
{
	return err_ != 0;
}

}//CoAP

#include "error.hpp"

namespace CoAP{

const char* Error::name() const noexcept
{
	return "CoAP";
}

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
		case errc::code_invalid: 		return "invalid code";
		case errc::option_invalid: 		return "invalid option";
		case errc::option_out_of_order:	return "option out of order";
		case errc::option_repeated: 	return "option repeated";
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

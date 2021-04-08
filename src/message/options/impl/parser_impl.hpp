#ifndef COAP_TE_MESSAGE_OPTIONS_PARSER_IMPL_HPP__
#define COAP_TE_MESSAGE_OPTIONS_PARSER_IMPL_HPP__

#include "../parser.hpp"
#include "../types.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode,
		bool CheckOptions /* = true */>
unsigned parse(option_template<OptionCode>& opt,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		unsigned delta,
		CoAP::Error& ec) noexcept
{
	unsigned offset = 0, code, length;

	code = buffer[0] >> 4;
	length = buffer[0] & 0x0F;
	offset += 1;

	switch(static_cast<delta_special>(code))
	{
		case delta_special::one_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 1)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.ocode = static_cast<OptionCode>(delta + buffer[1] + code);
			offset += 1;
			break;
		case delta_special::two_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 2)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.ocode = static_cast<OptionCode>(delta + (buffer[1] << 8 | buffer[2]) + code);
			offset += 2;
			break;
		case delta_special::error:
			ec = CoAP::errc::option_parse_error;
			return offset;
		default:
			opt.ocode = static_cast<OptionCode>(delta + code);
			break;
	}

	if constexpr(CheckOptions)
	{
		config<OptionCode> const* config = get_config<OptionCode>(opt.ocode);
		if(!config)
		{
			ec = CoAP::errc::option_invalid;
			return offset;
		}
	}

	switch(static_cast<length_special>(length))
	{
		case length_special::one_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 1)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.length = buffer[offset] + 13;
			offset += 1;
			break;
		case length_special::two_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 2)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.length = (buffer[offset] << 8 | buffer[offset + 1]) + 269;
			offset += 2;
			break;
		case length_special::error:
			ec = CoAP::errc::option_parse_error;
			return offset;
		default:
			opt.length = length;
			break;
	}
	if(opt.length)
	{
		if constexpr(CheckOptions)
		{
			config<OptionCode> const* config = get_config<OptionCode>(opt.ocode);
			if(config && config->otype == Option::type::empty)
			{
				ec = CoAP::errc::option_parse_error;
				return offset;
			}
			if((buffer_len - offset) < opt.length)
			{
				ec = CoAP::errc::message_too_small;
				return offset;
			}
		}
		opt.value = &buffer[offset];
		offset += opt.length;
	}
	return offset;
}

template<typename OptionCode>
template<typename Message>
Parser<OptionCode>::Parser(Message const& msg) // @suppress("Member declaration not found")
	: length_(msg.options_len), init_(msg.option_init), total_num_(msg.option_num){}

template<typename OptionCode>
void Parser<OptionCode>::reset() noexcept
{
	current_opt_ = 0;
	delta_ = 0;
	offset_ = 0;
}

template<typename OptionCode>
option_template<OptionCode> const* Parser<OptionCode>::current() const noexcept
{
	return &opt_;
}

template<typename OptionCode>
unsigned Parser<OptionCode>::current_number() const noexcept
{
	return current_opt_;
}

template<typename OptionCode>
std::size_t Parser<OptionCode>::total_number() const noexcept
{
	return total_num_;
}

template<typename OptionCode>
unsigned Parser<OptionCode>::offset() const noexcept
{
	return offset_;
}

template<typename OptionCode>
template<bool CheckOptions /* = false */>
option_template<OptionCode> const*
Parser<OptionCode>::next() noexcept
{
	CoAP::Error ec;
	if((length_ - offset_) != 0)
	{
		offset_ += parse<OptionCode, CheckOptions>(opt_, init_ + offset_,
															length_ - offset_,
															delta_, ec);
		if(ec)
		{
			reset();
			return nullptr;
		}
		delta_ = static_cast<unsigned>(opt_.ocode);
		current_opt_++;
	} else return nullptr;

	return &opt_;
}

template<typename OptionCode>
template<bool CheckOptions /* = true */>
option_template<OptionCode> const*
Parser<OptionCode>::next(CoAP::Error& ec) noexcept
{
	if((length_ - offset_) != 0)
	{
		offset_ += parse<OptionCode, CheckOptions>(opt_, init_ + offset_,
															length_ - offset_,
															delta_, ec);
		if(ec)
			return nullptr;

		delta_ = static_cast<unsigned>(opt_.ocode);
		current_opt_++;
	} else return nullptr;

	return &opt_;
}

template<typename OptionCode,
		typename Message>
bool get_option(Message const& msg,
		option_template<OptionCode>& opt,
		OptionCode ocode, unsigned count /* = 0 */) noexcept
{
	Parser<OptionCode> parser(msg);
	option_template<OptionCode> const* op;
	unsigned c = 0;
	while((op = parser.next()))
	{
		if(op->ocode == ocode)
		{
			if(c++ == count)
			{
				opt = *op;
				return true;
			}
		}
	}
	return false;
}

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_PARSER_IMPL_HPP__ */

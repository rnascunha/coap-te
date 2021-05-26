#include "print_message.hpp"
#include <cstdio>
#include <cstring>

#include "message/parser.hpp"
#include "message/reliable/parser.hpp"
#include "message/options/options.hpp"
#include "message/options/parser.hpp"
#include "helper.hpp"
#include "output_string.hpp"
#include "print_options.hpp"

namespace CoAP{
namespace Debug{

void print_message(CoAP::Message::message const& msg) noexcept
{
	std::printf("\tType: %s\n\tCode: %s\n\tMessage ID: 0x%04X\n\tToken[%zu]: ",
			type_string(msg.mtype), code_string(msg.mcode), msg.mid, msg.token_len);
	print_array(msg.token, msg.token_len);

	std::printf("\n\tOptions[%zu]:\n", msg.option_num);
	unsigned offset = 0, delta = 0;
	for(std::size_t i = 0; i < msg.option_num; i++)
	{
		CoAP::Message::Option::option opt;
		CoAP::Error ec;
		offset += CoAP::Message::Option::parse<CoAP::Message::Option::code, true>(opt,
				msg.option_init + offset, msg.options_len - offset, delta, ec);

		delta = static_cast<unsigned>(opt.ocode);
		std::printf("\t\t");
		print_option(opt);
		std::printf("\n");
	}

	std::printf("\tPayload[%zu]: ", msg.payload_len);
	print_array(msg.payload, msg.payload_len);
	std::printf("\n");
}

bool print_message(std::uint8_t const* const arr, std::size_t size) noexcept
{
	CoAP::Message::message msg;
	CoAP::Error ec;
	CoAP::Message::parse(msg, arr, size, ec);
	if(ec) return false;

	std::printf("\tType: %s\n\tCode: %s\n\tMessage ID: 0x%04X\n\tToken[%zu]: ",
			type_string(msg.mtype), code_string(msg.mcode), msg.mid, msg.token_len);
	print_array(msg.token, msg.token_len);
	std::printf("\n\tOptions[%zu]:\n", msg.option_num);
	unsigned offset = 0, delta = 0;
	for(std::size_t i = 0; i < msg.option_num; i++)
	{
		CoAP::Message::Option::option opt;
		CoAP::Error ec_;
		offset += CoAP::Message::Option::parse<CoAP::Message::Option::code, false>(opt,
				msg.option_init + offset, msg.options_len - offset, delta, ec_);
		delta = static_cast<unsigned>(opt.ocode);
		std::printf("\t\t");
		print_option(opt);
		std::printf("\n");
	}
	std::printf("\tPayload[%zu]: ", msg.payload_len);
	print_array(msg.payload, msg.payload_len);
	std::printf("\n");

	return true;
}

void print_message_encoded_string(CoAP::Message::message const& msg) noexcept
{
	std::printf("\tType: %s\n\tCode: %s\n\tMessage ID: 0x%04X\n\tToken[%zu]: ",
			type_string(msg.mtype), code_string(msg.mcode), msg.mid, msg.token_len);
	print_array_as_string(msg.token, msg.token_len);
	std::printf("\n\tOptions[%zu]:\n", msg.option_num);
	unsigned offset = 0, delta = 0;
	for(std::size_t i = 0; i < msg.option_num; i++)
	{
		CoAP::Message::Option::option opt;
		CoAP::Error ec;
		offset += CoAP::Message::Option::parse<CoAP::Message::Option::code, true>(opt,
				msg.option_init + offset, msg.options_len - offset, delta, ec);
		if(ec)
		{
			std::printf("\n\nERROR PARSING[%d] %s\n\n", ec.value(), ec.message());
			std::printf("opt: %d/%u\n", static_cast<int>(opt.ocode), opt.length);
			exit(1);
		}
		delta = static_cast<unsigned>(opt.ocode);
		std::printf("\t\t");
		print_option(opt);
		std::printf("\n");
	}
	std::printf("\tPayload[%zu]: ", msg.payload_len);
	print_array_as_string(msg.payload, msg.payload_len);
	std::printf("\n");
}

void print_message_string(CoAP::Message::message const& msg) noexcept
{
	std::printf("\tType: %s\n\tCode: %s\n\tMessage ID: 0x%04X\n\tToken[%zu]: ",
			type_string(msg.mtype), code_string(msg.mcode), msg.mid, msg.token_len);
	print_array_as_string(msg.token, msg.token_len);
	std::printf("\n\tOptions[%zu]:\n", msg.option_num);
	unsigned offset = 0, delta = 0;
	for(std::size_t i = 0; i < msg.option_num; i++)
	{
		CoAP::Message::Option::option opt;
		CoAP::Error ec;
		offset += CoAP::Message::Option::parse<CoAP::Message::Option::code, true>(opt,
				msg.option_init + offset, msg.options_len - offset, delta, ec);
		if(ec)
		{
			std::printf("\n\nERROR PARSING[%d] %s\n\n", ec.value(), ec.message());
			std::printf("opt: %d/%u\n", static_cast<int>(opt.ocode), opt.length);
			exit(1);
		}
		delta = static_cast<unsigned>(opt.ocode);
		std::printf("\t\t");
		print_option(opt);
		std::printf("\n");
	}
	std::printf("\tPayload[%zu]: ", msg.payload_len);
	std::printf("%.*s", static_cast<int>(msg.payload_len), static_cast<char const*>(msg.payload));
	std::printf("\n");
}

static void print_make_space(const char* header, int size_data) noexcept
{
	int size_header = static_cast<int>(std::strlen(header));
	int w_size = size_header > (size_data) ? size_data : size_header;
	int diff = (w_size < size_data) ? size_data - w_size : 0;
	int diff_a = 0, diff_b = 0;
	if(diff)
	{
		diff_a = diff / 2;
		diff_b = diff / 2 + ((diff % 2) ? 1 : 0);
	}

	std::printf("|");
	while(diff_a--) std::printf(" ");
	std::printf("%.*s", size_data, header);
	while(diff_b--) std::printf(" ");
}

bool print_byte_message(std::uint8_t const* arr, std::size_t size) noexcept
{
	CoAP::Message::message msg;
	CoAP::Error ec;
	CoAP::Message::parse(msg, arr, size, ec);
	if(ec) return false;

	//Print data;
	std::printf("%02X|%02X|%04X", arr[0], arr[1], msg.mid);
	int s_token = 0, s_options = 0, s_payload = 0;
	if(msg.token_len)
	{
		std::printf("|");
		s_token = print_array(msg.token, msg.token_len, 2);
	}
	if(msg.options_len)
	{
		std::printf("|");
		s_options = print_array(msg.option_init, msg.options_len, 2);
	}
	if(msg.payload_len)
	{
		std::printf("|FF|");
		s_payload = print_array(msg.payload, msg.payload_len, 2);
	}
	std::printf("\n");

	//Making header
	std::printf("He|Co|MeID");
	char temp[15];
	if(msg.token_len)
	{
		std::snprintf(temp, 15, "Token[%zu]", msg.token_len);
		print_make_space(temp, s_token);
	}
	if(msg.options_len)
	{
		std::snprintf(temp, 15, "Options[%zu][%zu]", msg.options_len, msg.option_num);
		print_make_space(temp, s_options);
	}
	if(msg.payload_len)
	{
		printf("|MP");
		std::snprintf(temp, 15, "Payload[%zu]", msg.payload_len);
		print_make_space(temp, s_payload);
	}
	std::printf("\n");

	return true;
}

#if COAP_TE_RELIABLE_CONNECTION == 1

template<typename OptionCode>
static void print_message_options(CoAP::Message::Reliable::message const& msg,
		const char* prefix = "\t\t") noexcept
{
	CoAP::Message::Option::Parser<OptionCode> parser(msg);
	CoAP::Message::Option::option_template<OptionCode> const* opt;
	while((opt = parser.next()))
	{
		std::printf("%s", prefix);
		print_option(*opt, true);
		std::printf("\n");
	}
}

static void print_option_choose(CoAP::Message::Reliable::message const& msg) noexcept
{
	if(CoAP::Message::is_signaling(msg.mcode))
	{
		switch(msg.mcode)
		{
			case CoAP::Message::code::csm:
				print_message_options<CoAP::Message::Option::csm>(msg);
				break;
			case CoAP::Message::code::ping:
			case CoAP::Message::code::pong:
				print_message_options<CoAP::Message::Option::ping_pong>(msg);
				break;
			case CoAP::Message::code::release:
				print_message_options<CoAP::Message::Option::release>(msg);
				break;
			case CoAP::Message::code::abort:
				print_message_options<CoAP::Message::Option::abort>(msg);
				break;
			default:
				break;
		}
		return;
	}
	print_message_options<CoAP::Message::Option::code>(msg);
}

void print_message(CoAP::Message::Reliable::message const& msg) noexcept
{
	std::printf("\tLen: %u\n\tCode: %s\n\tToken[%zu]: ",
			msg.len, code_string(msg.mcode), msg.token_len);
	print_array(msg.token, msg.token_len);

	std::printf("\n\tOptions[%zu]:\n", msg.option_num);
	print_option_choose(msg);
	std::printf("\tPayload[%zu]: ", msg.payload_len);
	print_array(msg.payload, msg.payload_len);
	std::printf("\n");
}

void print_message_string(CoAP::Message::Reliable::message const& msg) noexcept
{
	std::printf("\tLen: %u\n\tCode: %s\n\tToken[%zu]: ",
			msg.len, code_string(msg.mcode), msg.token_len);
	print_array(msg.token, msg.token_len);

	std::printf("\n\tOptions[%zu]:\n", msg.option_num);
	print_option_choose(msg);
	std::printf("\tPayload[%zu]: ", msg.payload_len);
	std::printf("%.*s", static_cast<int>(msg.payload_len), static_cast<char const*>(msg.payload));
	std::printf("\n");
}

bool print_byte_reliable_message(std::uint8_t const* arr, std::size_t size) noexcept
{
	CoAP::Message::Reliable::message msg;
	CoAP::Error ec;
	CoAP::Message::Reliable::parse(msg, arr, size, ec);
	if(ec) return false;

	printf("%02X", arr[0]);
	int index = 0;
	if(msg.len >= 13)
		printf("|%02X", arr[++index]);
	if(msg.len >= 269)
		printf("|%02X", arr[++index]);
	if(msg.len >= 65805)
		printf("|%02X", arr[++index]);

	//Code
	printf("|%02X", arr[index + 1]);

	int s_token = 0, s_options = 0, s_payload = 0;
	if(msg.token_len)
	{
		printf("|");
		s_token = print_array(msg.token, msg.token_len, 2);
	}
	if(msg.options_len)
	{
		printf("|");
		s_options = print_array(msg.option_init, msg.options_len, 2);
	}
	if(msg.payload_len)
	{
		std::printf("|FF|");
		s_payload = print_array(msg.payload, msg.payload_len, 2);
	}
	std::printf("\n");

	//Making header
	std::printf("He");
	char temp[15];
	if(index)
	{
		std::snprintf(temp, 15, "Ext[%u]", index);
		print_make_space(temp, 2 * index);
	}
	//Code
	std::printf("|Co");

	if(msg.token_len)
	{
		std::snprintf(temp, 15, "Token[%zu]", msg.token_len);
		print_make_space(temp, s_token);
	}
	if(msg.options_len)
	{
		std::snprintf(temp, 15, "Options[%zu][%zu]", msg.options_len, msg.option_num);
		print_make_space(temp, s_options);
	}
	if(msg.payload_len)
	{
		std::printf("|MP");
		std::snprintf(temp, 15, "Payload[%zu]", msg.payload_len);
		print_make_space(temp, s_payload);
	}
	std::printf("\n");

	return true;
}

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Debug
}//CoAP

#include "print_message.hpp"
#include <cstdio>
#include <cstring>

#include "message/parser.hpp"
#include "helper.hpp"
#include "output_string.hpp"
#include "print_options.hpp"

namespace CoAP{
namespace Debug{

void print_message(CoAP::Message::message const& msg)
{
	std::printf("\tType: %s\n\tCode: %s\n\tMessage ID: 0x%04X\n\tToken[%lu]: ",
			type_string(msg.mtype), code_string(msg.mcode), msg.mid, msg.token_len);
	print_array(msg.token, msg.token_len);

	std::printf("\n\tOptions[%lu]:\n", msg.option_num);
	unsigned offset = 0, delta = 0;
	for(std::size_t i = 0; i < msg.option_num; i++)
	{
		CoAP::Message::Option::option opt;
		CoAP::Error ec;
		offset += CoAP::Message::parse_option<true>(opt,
				msg.option_init + offset, msg.options_len - offset, delta, ec);

		delta = static_cast<unsigned>(opt.ocode);
		std::printf("\t\t");
		print_option(opt);
		std::printf("\n");
	}

	std::printf("\tPayload[%lu]: ", msg.payload_len);
	print_array(msg.payload, msg.payload_len);
	std::printf("\n");
}

bool print_message(std::uint8_t const* const arr, std::size_t size)
{
	CoAP::Message::message msg;
	CoAP::Error ec;
	CoAP::Message::parse(msg, arr, size, ec);
	if(ec) return false;

	std::printf("\tType: %s\n\tCode: %s\n\tMessage ID: 0x%04X\n\tToken[%lu]: ",
			type_string(msg.mtype), code_string(msg.mcode), msg.mid, msg.token_len);
	print_array(msg.token, msg.token_len);
	std::printf("\n\tOptions[%lu]:\n", msg.option_num);
	unsigned offset = 0, delta = 0;
	for(std::size_t i = 0; i < msg.option_num; i++)
	{
		CoAP::Message::Option::option opt;
		CoAP::Error ec;
		offset += CoAP::Message::parse_option<false>(opt,
				msg.option_init + offset, msg.options_len - offset, delta, ec);
		delta = static_cast<unsigned>(opt.ocode);
		std::printf("\t\t");
		print_option(opt);
		std::printf("\n");
	}
	std::printf("\tPayload[%lu]: ", msg.payload_len);
	print_array(msg.payload, msg.payload_len);
	std::printf("\n");

	return true;
}

void print_message_str(CoAP::Message::message const& msg) noexcept
{
	std::printf("\tType: %s\n\tCode: %s\n\tMessage ID: 0x%04X\n\tToken[%lu]: ",
			type_string(msg.mtype), code_string(msg.mcode), msg.mid, msg.token_len);
	print_array_as_string(msg.token, msg.token_len);
	std::printf("\n\tOptions[%lu]:\n", msg.option_num);
	unsigned offset = 0, delta = 0;
	for(std::size_t i = 0; i < msg.option_num; i++)
	{
		CoAP::Message::Option::option opt;
		CoAP::Error ec;
		offset += CoAP::Message::parse_option<true>(opt,
				msg.option_init + offset, msg.options_len - offset, delta, ec);
		if(ec)
		{
			printf("\n\nERROR PARSING[%d] %s\n\n", ec.value(), ec.message());
			printf("opt: %d/%u\n", static_cast<int>(opt.ocode), opt.length);
			exit(1);
		}
		delta = static_cast<unsigned>(opt.ocode);
		std::printf("\t\t");
		print_option(opt);
		std::printf("\n");
	}
	std::printf("\tPayload[%lu]: ", msg.payload_len);
	print_array_as_string(msg.payload, msg.payload_len);
	std::printf("\n");
}

static void print_make_space(const char* header, std::size_t length, int size_data)
{
	int size_header = std::strlen(header);
	int w_size = size_header > (size_data) ? size_data : size_header;
	int diff = (w_size < size_data) ? size_data - w_size : 0;
	int diff_a = 0, diff_b = 0;
	if(diff)
	{
		diff_a = diff / 2;
		diff_b = diff / 2 + ((diff % 2) ? 1 : 0);
	}

	printf("|");
	while(diff_a--) printf(" ");
	printf("%.*s", size_data, header);
	while(diff_b--) printf(" ");
}

bool print_byte_message(std::uint8_t const* const arr, std::size_t size)
{
	CoAP::Message::message msg;
	CoAP::Error ec;
	CoAP::Message::parse(msg, arr, size, ec);
	if(ec) return false;

	//Print data;
	printf("%02X|%02X|%04X", arr[0], arr[1], msg.mid);
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
		printf("|");
		s_payload = print_array(msg.payload, msg.payload_len, 2);
	}
	printf("\n");

	//Making header
	printf("He|Co|MeID");
	char temp[15];
	if(msg.token_len)
	{
		std::snprintf(temp, 15, "Token[%lu]", msg.token_len);
		print_make_space(temp, msg.token_len, s_token);
	}
	if(msg.options_len)
	{
		std::snprintf(temp, 15, "Options[%lu][%lu]", msg.options_len, msg.option_num);
		print_make_space(temp, msg.options_len, s_options);
	}
	if(msg.payload_len)
	{
		std::snprintf(temp, 15, "Payload[%lu]", msg.payload_len);
		print_make_space(temp, msg.payload_len, s_payload);
	}
	printf("\n");

	return true;
}

}//Debug
}//CoAP

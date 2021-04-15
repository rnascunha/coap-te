/**
 * This examples show how to use the no response option defined
 * at RFC7967
 *
 * We are going to serialize a message and than parse it and check if
 * the message should be or not suppressed (if this was a server that
 * received the message).
 */

#include <cstdint>

#include "log.hpp"			//Log
#include "coap-te.hpp"		//Convenient header
#include "coap-te-debug.hpp" //Convenient debug header

#define BUFFER_LEN		512	//Buffer length

using namespace CoAP::Message;
using namespace CoAP::Log;

/**
 * Log module
 */
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
};

/**
 * Auxiliar function
 */
static void exit_error(CoAP::Error& ec, const char* what = nullptr) noexcept
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

/**
 * Print suppress information
 */
static void print_message(code mcode, bool suppress) noexcept
{
	if(!suppress)
		status(example_mod, "Not suppress %s message",
			CoAP::Debug::code_string(mcode));
	else
		warning(example_mod, "Suppress %s message",
					CoAP::Debug::code_string(mcode));
}

int main()
{
	status(example_mod, "No response example...");

	std::uint8_t buffer[BUFFER_LEN];	//Buffer where the data will be serialized
	CoAP::Error ec;						//Use to error report

	message_id mid(CoAP::time());

	Factory<> fac;

	/**
	 * We are going to suppress any error message
	 */
	Option::suppress sup = Option::suppress::client_error | Option::suppress::server_error;
	Option::node no_response_op{sup};

	fac
		.header(CoAP::Message::type::nonconfirmable, code::get)
		.add_option(no_response_op);

	status(example_mod, "Serializing...");
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, mid(), ec);
	if(ec) exit_error(ec, "serialize");

	/**
	 * Now we are gonna parse the message that we just serialized
	 * (supposed we received from network)
	 */
	status(example_mod, "Parsing message...");

	/**
	 * Parsing message...
	 */
	message msg;
	unsigned size_parse = parse(msg, buffer, size, ec);
	if(ec) exit_error(ec, "parsing");
	/**
	 * Parsed succeeded!
	 */
	status(example_mod, "Parsing succeded! size=%u...", size_parse);
	debug(example_mod, "Printing message...");
	CoAP::Debug::print_message(msg);

	/**
	 * There is 2 ways to check if a message should be suppressed
	 *
	 * \note I the message doesn't have the no response option, the above functions will
	 * return 'false' to suppress message.
	 */
	/**
	 * If you are going to check just one time, you can use
	 * this method
	 */
	print_message(code::content, Option::no_response_suppress(msg, code::content));
	/**
	 * But if we are going to test lots fo codes, is better to first get the suppress
	 * value and than compare.
	 */
	Option::suppress value = Option::get_suppress_value(msg);
	print_message(code::bad_gateway, Option::no_response_suppress(value, code::bad_gateway));
	print_message(code::unauthorized, Option::no_response_suppress(value, code::unauthorized));
	print_message(code::changed, Option::no_response_suppress(value, code::changed));
	print_message(code::service_unavaiable, Option::no_response_suppress(value, code::service_unavaiable));

	return EXIT_SUCCESS;
}

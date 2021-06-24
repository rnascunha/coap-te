/**
 * Factory examples show how to use... a factory... to construct
 * a message.
 *
 * Factorys can be instantiated as follows:
 * * with/without internal buffer;
 * * with/without internal message id generator;
 *
 * Even if you use internal buffer/message id, you still can pass
 * the arguments externally.
 */

#include <cstdint>

#include "coap-te/log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

#define BUFFER_LEN		512		//Size of buffer

/**
 * Comment/uncomment the following line to use/not use the internal
 * buffer of the factory.
 */
#define USE_INTERNAL_BUFFER

/**
 * Comment/uncomment the following line to use/not use the internal
 * message id generator of the factory
 */
#define USE_INTERNAL_MESSAGE_ID

using namespace CoAP::Message;
using namespace CoAP::Log;

/**
 * Log module
 */
static constexpr module example_mod = {
		/*.name = */"EXAMPLE",
		/*.max_level = */CoAP::Log::type::debug
};

/**
 * Auxiliary function
 */
static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

int main()
{
#ifndef USE_INTERNAL_BUFFER
	/**
	 * If we are not going to use internal buffer, we need one
	 */
	std::uint8_t buffer[BUFFER_LEN];
#endif /* USE_INTERNAL_BUFFER */
#ifndef USE_INTERNAL_MESSAGE_ID
	/**
	 * If we are not going to use a internal message id generator,
	 * we need to instantiate one
	 */
	message_id mid((unsigned)CoAP::time());
#endif /* USE_INTERNAL_MESSAGE_ID */
	CoAP::Error ec;		//message error

	/**
	 * All object lifetime if of user responsability. Util "serialize" method
	 * been called, nothing is copied.
	 */
	/**
	 * Token
	 */
	std::uint8_t token[] = {0x03, 0x04, 0x05, 0x06, 0x07};

	/**
	 * Options node. Factory uses option list internally, so we
	 * need to instantiate options node
	 */
	CoAP::Message::accept format{accept::application_json};
	Option::node content_op{format, true};
	Option::node path_op1{Option::code::uri_path, "path1"};
	Option::node path_op2{Option::code::uri_query, "value=1"};

	status(example_mod, "Testing factory...");

	/**
	 * The following lines show how to instantiate a factory with/without
	 * internal features.
	 */
#if defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	status(example_mod, "Internal buffer / Internal message id");
	Factory<BUFFER_LEN, message_id> fac{message_id((unsigned)CoAP::time())};
#elif defined(USE_INTERNAL_BUFFER) && !defined(USE_INTERNAL_MESSAGE_ID)
	status(example_mod, "Internal buffer / NO internal message id");
	Factory<BUFFER_LEN> fac;
#elif !defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	/**
	 * 0 buffer len means NO internal buffer
	 */
	status(example_mod, "NO internal buffer / Internal message id");
	Factory<0, message_id> fac{message_id((unsigned)CoAP::time())};
#else
	/**
	 * 0 buffer len means NO internal buffer
	 */
	status(example_mod, "NO internal buffer / NO internal message id\n");
	Factory<0> fac;
#endif

	/**
	 * Adding all header/token/options/payload to the factory
	 */
	fac
		.header(CoAP::Message::type::confirmable, code::get)
		.token(token, sizeof(token))
		.add_option(content_op)
		.add_option(path_op1)
		.add_option(path_op2)
		.payload("payload");

	/**
	 * When factory doesn't have a buffer/message id internally, you
	 * must provide externally
	 */
#if defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	std::size_t size = fac.serialize(ec);
#elif defined(USE_INTERNAL_BUFFER) && !defined(USE_INTERNAL_MESSAGE_ID)
	std::size_t size = fac.serialize(mid(), ec);
#elif !defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, ec);
#else
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, mid(), ec);
#endif
	/**
	 * Checking serialize
	 */
	if(ec) exit_error(ec, "serialize");
	/**
	 * After serialize, you can use the factory again, but first you need to call reset
	 *
	 * fac.reset();
	 * .
	 * .
	 * .
	 */

	debug(example_mod, "------------");

	/**
	 * Serialize succeeded
	 */
	status(example_mod, "Serialize succeeded! [%lu]...", size);
	debug(example_mod, "Printing message bytes...");
#ifndef USE_INTERNAL_BUFFER
	CoAP::Debug::print_byte_message(buffer, size);
#else /* USE_INTERNAL_BUFFER */
	CoAP::Debug::print_byte_message(fac.buffer(), size);
#endif

	debug(example_mod, "-----------");
	status(example_mod, "Parsing message...");

	/**
	 * Structure to hold message parsed
	 */
	message msg;
#ifndef USE_INTERNAL_BUFFER
	auto size_parse = parse(msg, buffer, size, ec);
#else /* USE_INTERNAL_BUFFER */
	auto size_parse = parse(msg, fac.buffer(), size, ec);
#endif /* USE_INTERNAL_BUFFER */
	/**
	 * Checking parse
	 */
	if(ec) exit_error(ec, "parsing");
	/**
	 * Parse succeeded!
	 */
	status(example_mod, "Parsing succeeded! [%u]...", size_parse);
	debug(example_mod, "Printing message...");
	CoAP::Debug::print_message(msg);

	status(example_mod, "Success!");
	return EXIT_SUCCESS;
}

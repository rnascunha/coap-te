/**
 * The serialize/parse example shows how to... serialize a parse
 * a message. It uses different strategies to how to build a message,
 * and follows parsing and accessing this same message. At the end
 * it shows how to iterate through the message options.
 *
 * \note serialize is the act of write at the buffer all data to be sent,
 * as defined by the CoAP protocol.
 */

#include <cstdint>

#include "coap-te/log.hpp"			//Log
#include "coap-te.hpp"		//Convenient header
#include "coap-te-debug.hpp" //Convenient debug header

#define BUFFER_LEN		512	//Buffer length

/**
 * The following defines changes how to serialize a message, but the
 * final result is the same.
 */
/**
 * If defined shows the use of factory (more about factory at the
 * 'factory' example). Commenting this line will show how to manually
 * serialize a message.
 */
#define TEST_FACTORY
/**
 * When manually building a message, to include all options, you can
 * make a list or use a array. Let it uncommented to see how to use a list
 */
#define TEST_LIST

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
 * Auxiliar function
 */
static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

int main()
{
	status(example_mod, "Serialize/Parse Example...");

	std::uint8_t buffer[BUFFER_LEN];	//Buffer where the data will be serialized
	CoAP::Error ec;						//Use to error report

	//Message token
	std::uint8_t token[] = {0x03, 0x04, 0x05, 0x06, 0x07};
	//Message ID generator
	message_id mid((unsigned int)CoAP::time());


	CoAP::Message::accept content = accept::application_json;
	const char* payload = "my data";

#if !defined(TEST_LIST) && !defined(TEST_FACTORY)
	status(example_mod, "Testing options array...");

	/**
	 * Declaring a array of options
	 */
	Option::option options[] = {
			{content},
			{Option::code::uri_path, "sensor"},
			{Option::code::uri_path, "temp"}
	};

	status(example_mod, "Serializing...");
	/**
	 * All data must be passed to the serialize function to copy
	 * correctly to the buffer
	 *
	 * token/options and payload are not mandatory. If your message
	 * doesn't need then, use:
	 * * token/token_len = nullptr/0;
	 * * list: just don't add any option to the list;
	 * * payload/payload_len = nullptr/0;
	 */
	std::size_t size = serialize(
			buffer, BUFFER_LEN,					//Buffer/buffer length where data will be serialize
			CoAP::Message::type::confirmable,	//Message type (check message/types.hpp)
			code::get,							//Message code (check message/codes.hpp)
			mid(),								//Message ID generator
			token, sizeof(token),				//Token and token size
			options, sizeof(options) / sizeof(Option::option), //Option array and length
			payload, std::strlen(payload),		//Payload/payload length
			ec);
#elif defined(TEST_FACTORY)
	status(example_mod, "Testing factory...");

	/*
	 * Instantiation of factory (more about the template arguments at the
	 * 'factory' example.
	 */
	Factory<> fac;

	/**
	 * Making the header. Defining the type of message (confirmable) and method (get)
	 * Also adding the token.
	 *
	 * All messages types can be checked at message/types.hpp
	 * All messages code can be checked at message/codes.hpp
	 */
	fac.header(CoAP::Message::type::confirmable, code::get);
	fac.token(token, sizeof(token));

	/**
	 * As internally factory uses lists, declaring a option node with
	 * all options to be include. Until serialize function be called,
	 * is of the user responsibility the objects lifetime
	 */
	Option::node content_op{content};
	Option::node path1_op{Option::code::uri_path, "sensor"};
	Option::node path2_op{Option::code::uri_path, "temp"};

	/**
	 * Adding each option. All options are added at the correct
	 * position
	 */
	fac.add_option(content_op);
	fac.add_option(path1_op);
	fac.add_option(path2_op);

	/**
	 * Adding a payload
	 */
	fac.payload(payload);

	status(example_mod, "Serializing factory...");
	/**
	 * Serialize data to the buffer
	 *
	 * * 'mid' call with generate a message id to the message
	 * * 'ec' will output any error at trying to serialize. Always check
	 * for error
	 * * Serialize returns how many bytes where serialized (don't use this data
	 * to infer that serialization occur correctly).
	 */
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, mid(), ec);

	/**
	 * After serialize, you can use the factory again, but first you need to call reset
	 *
	 * fac.reset();
	 * .
	 * .
	 * .
	 */
#else
	status(example_mod, "Testing options list...");

	/**
	 * Declaring option list
	 */
	Option::List list;

	/**
	 * Declaring a option node with all options to be include.
	 * Until serialize function be called, is of the user responsibility
	 * the objects lifetime
	 */
	Option::node content_op{content};
	Option::node path1_op{Option::code::uri_path, "sensor"};
	Option::node path2_op{Option::code::uri_path, "temp"};

	/**
	 * Adding options to the list
	 */
	list.add(content_op);
	list.add(path1_op);
	list.add(path2_op);

	status(example_mod, "Serializing...");
	/**
	 * All data must be passed to the serialize function to copy
	 * correctly to the buffer
	 *
	 * token/options and payload are not mandatory. If your message
	 * doesn't need then, use:
	 * * token/token_len = nullptr/0;
	 * * list: just don't add any option to the list;
	 * * payload/payload_len = nullptr/0;
	 */
	std::size_t size = serialize(
			buffer, BUFFER_LEN,					//Buffer/buffer length where data will be serialize
			CoAP::Message::type::confirmable,	//Message type (check message/types.hpp)
			code::get,							//Message code (check message/codes.hpp)
			mid(),								//Message ID generator
			token, sizeof(token),				//Token and token size
			list,								//The option list declared above
			payload, std::strlen(payload),		//Payload/payload length
			ec);
#endif /* TEST_LIST */
	debug(example_mod, "------------");
	/**
	 * Checking serialize success
	 */
	if(ec) exit_error(ec, "serialize");

	/**
	 * Serialize succeeded
	 */
	debug(example_mod, "Serialize succeeded! size=%lu...", size);
	debug(example_mod, "Printing message bytes...");
	CoAP::Debug::print_byte_message(buffer, size);

	/**
	 * Now we are gonna parse the message that we just serialized
	 * (supposed we received from network)
	 */
	debug(example_mod, "-------------");
	status(example_mod, "Parsing message...");

	/**
	 * message struct will hold the information parsed.
	 *
	 * It doesn't copy any internally (except trivial values), it
	 * just point to the buffer where the information is.
	 *
	 * check message/types.hpp (struct message) to examine each
	 * field of message structure.
	 */
	message msg;
	unsigned size_parse = parse(msg, buffer, size, ec);
	/**
	 * Checking parse
	 */
	if(ec) exit_error(ec, "parsing");
	/**
	 * Parsed succeeded!
	 */
	status(example_mod, "Parsing succeded! size=%u...", size_parse);
	debug(example_mod, "Printing message...");
	CoAP::Debug::print_message(msg);

	/**
	 * Showing how to iterate through options
	 */
	debug(example_mod, "--------------");
	status("Iterate options... num/size=%lu/%lu", msg.option_num, msg.options_len);

	/**
	 * Options are composed to 3 fields:
	 * * ocode: option code
	 * * length: option length
	 * * value: pointer to option value
	 *
	 * To verify all options code, see "message/options.hpp"
	 */
	Option::Parser<Option::code> op{msg};	//The parser will iterate through the options
	Option::option const* opt;				//This will hold a pointer to the option
	while((opt = op.next()))
	{
		CoAP::Debug::print_option(*opt);
		std::printf("\n");
	}

	status(example_mod, "Success!");

	return EXIT_SUCCESS;
}

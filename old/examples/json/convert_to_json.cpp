/**
 * This example we are going to show how to use a helper function
 * to create a JSON from a CoAP message.
 *
 * We use rapidjson library (https://github.com/Tencent/rapidjson) to
 * create the JSON. To enable this functions you must compile CoAP-te
 * with '-DRAPIDJSON_PATH=<path/to/rapidjson>/include' definition.
 *
 * Here we are going first to create and parse a CoAP message, and then
 * create the JSON, and stringify it.
 */

#include <cstdint>

#include "coap-te/log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

#include <string>

#define BUFFER_LEN		512		//Size of buffer

using namespace CoAP::Message;
using namespace CoAP::Log;

/**
 * Letting this definition, we are going to receive a JSON stringified
 * (std::string) directly. Commenting we are going to first receive the
 * JSON object (possibly modify), and then stringify.
 */
#define JSON_STRINGFY_SINGLE_STEP

#ifndef JSON_STRINGFY_SINGLE_STEP
#include "rapidjson/document.h"
#endif /* JSON_STRINGFY_SINGLE_STEP */

/**
 * Prettyfy or not the output
 */
#define PRETTYFY			true

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
	status(example_mod, "Internal buffer / Internal message id");
	Factory<BUFFER_LEN, message_id> fac{message_id((unsigned)CoAP::time())};

	fac
		.header(CoAP::Message::type::confirmable, code::get)
		.token(token, sizeof(token))
		.add_option(content_op)
		.add_option(path_op1)
		.add_option(path_op2)
		.payload("payload");

	std::size_t size = fac.serialize(ec);

	/**
	 * Checking serialize
	 */
	if(ec) exit_error(ec, "serialize");

	debug(example_mod, "------------");

	/**
	 * Serialize succeeded
	 */
	status(example_mod, "Serialize succeeded! [%lu]...", size);

	debug(example_mod, "-----------");
	status(example_mod, "Parsing message...");

	/**
	 * Structure to hold message parsed
	 */
	message msg;
	auto size_parse = parse(msg, fac.buffer(), size, ec);
	if(ec) exit_error(ec, "parsing");
	status(example_mod, "Parsing succeeded! [%u]...", size_parse);
	debug(example_mod, "Convert message to JSON format...");

	/**
	 * Here we are start to show how to use the convert to JSON functions
	 */
#ifdef JSON_STRINGFY_SINGLE_STEP
	/**
	 * Single step to receive a stringfied function
	 */
	std::printf("%s\n", CoAP::Helper::to_json<PRETTYFY>(msg).c_str());
#else /* JSON_STRINGFY_SINGLE_STEP */
	/**
	 * First receive the JSON document and then stringify
	 */
	rapidjson::Document doc;
	CoAP::Helper::to_json(doc, msg);

	std::string str{CoAP::Helper::stringify<PRETTYFY>(doc)};
	std::printf("%s\n", str.c_str());
#endif /* JSON_STRINGFY_SINGLE_STEP */

	return EXIT_SUCCESS;
}

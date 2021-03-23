/**
 * Here we are going to show how to use the 'Serialize' class.
 *
 * The 'Serialize' class is a class to manage messages that
 * where already serialized (written to a buffer). You will
 * be able to add/remove token, options and/or payload, or
 * also change the header.
 *
 * As we are going to change information that was already
 * hidden to the buffer, lots of memory shifts will have to be
 * performed, consuming lots of resources (cpu cycles :-p). So,
 * if you a creating a new message, prefer the use of a 'Factory'
 * or the 'serialize' free functions directly!
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
static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(ec, what);
	exit(EXIT_FAILURE);
}

static void exit_error(const char* what)
{
	error(what);
	exit(EXIT_FAILURE);
}

int main()
{
	status(example_mod, "Serialize class Example...");

	std::uint8_t buffer[BUFFER_LEN];	//Buffer where the data will be serialized
	CoAP::Error ec;						//Use to error report

	//Message ID generator
	message_id mid(CoAP::time());

	/**
	 * Instantiating options
	 */
	CoAP::Message::accept content = accept::application_json;
	unsigned size1 = 128;

	Option::node size1_op{Option::code::size1, size1};
	Option::node content_op{content};
	Option::node path1_op{Option::code::uri_path, "sensor"};
	Option::node path2_op{Option::code::uri_path, "temp"};
	Option::node query_op{Option::code::uri_query, "key=value"};
	Option::node query2_op{Option::code::uri_query, "key2=value2"};

	/*
	 * Create a message with a factory and serializing
	 */
	status(example_mod, "Serializing message...");
	Factory<> fac;
	std::size_t size = fac.header(CoAP::Message::type::nonconfirmable, code::get)
		.token("tok")
		.add_option(size1_op)
		.add_option(content_op)
		.add_option(path1_op)
		.add_option(path2_op)
		.add_option(query_op)
		.add_option(query2_op)
		.payload("my payload")
		.serialize(buffer, BUFFER_LEN, mid(), ec);

	if(ec) exit_error(ec, "serialize");

	/**
	 * Serialize succeeded
	 */
	debug(example_mod, "Serialize succeeded! size=%lu...", size);
	debug(example_mod, "Printing message bytes...");
	CoAP::Debug::print_byte_message(buffer, size);

	/**
	 * Let's imagine that we received this message. So, naturally,
	 * we are going to parse it.
	 */
	debug(example_mod, "-------------");
	status(example_mod, "Parsing message...");

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
	 * Every seems OK. But we need to forward this message to a server.
	 * But before we must add some information, and remove others. As
	 * the message is in a buffer, we can use the 'Serialize' class
	 *
	 * The second argument is the total size of the buffer, and not the
	 * size of the message received. This is necessary because with we
	 * add any information, we must know if this will fit at the buffer.
	 */
	Serialize serial(buffer, BUFFER_LEN, msg);

	/**
	 * Now lets make some modifications.
	 *
	 * We are going to change the type (NON->CON), keep the code and
	 * generate a new message id.
	 */
	status(example_mod, "Setting new header...");
	if(!serial.header(CoAP::Message::type::confirmable, msg.mcode, mid()))
		exit_error("header");

	/**
	 * And change the token
	 */
	status(example_mod, "Setting new token...");
	if(!serial.token("mytok"))
		exit_error("change token");

	/**
	 * Remove some options
	 */
	status(example_mod, "Removing options...");
	if(!serial.remove_option({Option::code::size1}))
		exit_error("option size1");

	/**
	 * Add our own
	 */
	status(example_mod, "Adding options...");
	unsigned n_size = 512;
	if(!serial.add_option({Option::code::size1, n_size}))
		exit_error("option size1");

	if(!serial.add_option({Option::code::uri_host, "10.10.10.11"}))
		exit_error("option host");

	unsigned port = 5683;
	if(!serial.add_option({Option::code::uri_port, port}))
		exit_error("option port");

	/**
	 * And change the payload
	 */
	status(example_mod, "Changing payload...");
	std::uint8_t load[] = {1, 2, 3, 4, 5, 6};
	if(!serial.payload(load, sizeof(load) / sizeof(load[0])))
		exit_error("payload");

	status(example_mod, "Printing new message [%lu]", serial.used());
	CoAP::Debug::print_message(serial.get_message());

	/**
	 * As you can see, we modified all the information at the buffer...
	 * There is no need to serialize, as everything is already written.
	 * You can just send the message:
	 * conn.send(serial.buffer(), serial.used(), some_endpoint, error);
	 */

	status(example_mod, "\n\nFinish It");

	return EXIT_SUCCESS;
}

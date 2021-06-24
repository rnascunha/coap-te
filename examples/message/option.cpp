/**
 * This examples will show how to manipulate 'options' to construct or
 * parse a message.
 *
 * Options can be included using a 'option list' or a 'option array'.
 * When using option list, you must create the nodes of the
 * list: 'CoAP::Message::Option::node'. When using a array, you must
 * create a array of options of type: 'CoAP::Message::Option::option'.
 * Factorys use internally option lists.
 *
 * To parse a message you have to use a Option_Parser.
 *
 * A option is composed of:
 * * ocode: code of the option;
 * * length: payload size of the option;
 * * value: the payload of option.
 *
 * This structured is shown at 'struct option' at 'message/options.hpp'.
 *
 * The payload of a option can be of types:
 * * empty: no payload;
 * * string;
 * * unsigned interger;
 * * opaque: random bytes;
 *
 * To parse the option value, you must know before hand which type it
 * is. The 'config' array at 'message/options.hpp' shows this.
 *
 * More about options:
 * https://tools.ietf.org/html/rfc7252#section-5.10
 */
#include <cstdint>
#include <cstdio>				//printf

#include "coap-te/log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

using namespace CoAP::Message;
using namespace CoAP::Log;

/**
 * Auxiliar function
 */
static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(ec, what);
	exit(EXIT_FAILURE);
}

int main()
{
	status("Options example...");

	/**
	 * We are going to create some options to our message.
	 *
	 * 'option' or 'node' are constructed the same way... but internally
	 * 'node's have some others structures to create the list. The constructor
	 * doesn't if the type is correct to that option (well, function 'create_option'
	 * does that... but for now, let's forget about this).
	 *
	 * We are just going to show the use of lists here, for brevity.
	 */

	/**
	 * option 'accept' or 'content_format' have a special constructor.
	 * The second argument must be true for accept and false for content_format.
	 */
	Option::node accept_op{CoAP::Message::accept::application_json, true};

	/**
	 * Uri_Path and Uri_Query are options of type 'string', and can be repeated.
	 */
	Option::node path1_op{Option::code::uri_path, "path1"},
				path2_op{Option::code::uri_path, "path2"},
				query1_op{Option::code::uri_query, "query=1"},
				query2_op{Option::code::uri_query, "my_query"},
				query3_op{Option::code::uri_query, "query3=3"};

	/**
	 * Option max-age is of type unsigned. The unsigned value must be
	 * declared separated, as the option doesn't hold any value, just
	 * reference to it;
	 */
	unsigned max_age = 60;
	Option::node max_age_op{Option::code::max_age, max_age};

	/**
	 * Adding a opaque value
	 */
	std::uint8_t if_match[] = {0x1, 0x2, 0x3};
	Option::node if_match_op{Option::code::if_match, if_match, 3};

	/**
	 * Adding a empty option
	 */
	Option::node none_if_match_op{Option::code::if_none_match};

	/**
	 * Here we are going to use a factory to construct our message, and
	 * serialize to a buffer. Until the call of 'serialize' is of the
	 * user responsibility the life time of the options declared below.
	 *
	 * The order of the options when serialize are defined by the option code;
	 * but don't worry, the serialize code will do it for us. It will also
	 * check if types that can not be repeat, repeat.
	 */
	std::uint8_t buffer[512];

	CoAP::Error ec;
	Factory<> fac;

	status("Serializing message...");
	std::size_t size = fac.header(CoAP::Message::type::confirmable, code::get)
		.add_option(accept_op)
		.add_option(path1_op)
		.add_option(path2_op)
		.add_option(query1_op)
		.add_option(query2_op)
		.add_option(query3_op)
		.add_option(if_match_op)
		.add_option(none_if_match_op)
		.add_option(max_age_op)
		.serialize(buffer, 512,
				0x1234,		//just setting a message id
				ec);

	if(ec) exit_error(ec, "serialize");

	status("Serialize success!");
	CoAP::Debug::print_byte_message(buffer, size);
	status("\n");
	/**
	 * Know lets parse the message
	 */
	message msg;
	status("Parsing message...");

	parse(msg, buffer, size, ec);
	if(ec) exit_error(ec, "parse");

	status("Parse success!");

	/**
	 * We are going to parse only the option. First, lets declare
	 * a Option_Parser
	 */
	Option::Parser<Option::code> parser{msg};
	Option::option const* opt;		//this will hold the option parsed.
	Option::config<Option::code> const* oconfig;	//this will hold option config.

	/**
	 * Let's iterate through the options and print it values
	 */
	status("Parsing and printing options...");
	while((opt = parser.next()))
	{
		oconfig = Option::get_config(opt->ocode);
		if(!oconfig) //same invalid option
		{
			std::printf("Invalid option: %u|len: %u\n", static_cast<unsigned>(opt->ocode), opt->length);
			continue;
		}
		/**
		 * Lets print based on type
		 */
		switch(oconfig->otype)
		{
			case Option::type::empty:
				std::printf("empty: %u|%s|len: %u\n",
						static_cast<unsigned>(opt->ocode),
						CoAP::Debug::option_string(opt->ocode),
						opt->length);
				break;
			case Option::type::opaque:
				std::printf("opaque: %u|%s|len: %u|payload: ",
						static_cast<unsigned>(opt->ocode),
						CoAP::Debug::option_string(opt->ocode),
						opt->length
						);
					CoAP::Debug::print_array(opt->value, opt->length);
					std::printf("\n");
				break;
			case Option::type::string:
				std::printf("string: %u|%s|len: %u|payload: %.*s\n",
						static_cast<unsigned>(opt->ocode),
						CoAP::Debug::option_string(opt->ocode),
						opt->length, opt->length, static_cast<char const*>(opt->value));
				break;
			case Option::type::uint:
				std::printf("uint: %u|%s|len: %u|payload: %u\n",
						static_cast<unsigned>(opt->ocode),
						CoAP::Debug::option_string(opt->ocode),
						opt->length, Option::parse_unsigned<Option::code>(*opt));
				break;
			default:
				break;
		}
	}

	return EXIT_SUCCESS;
}

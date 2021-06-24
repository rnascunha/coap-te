/**
 * This examples shows how to use Signaling of reliable
 * CoAP connection (RFC8323).
 *
 * At this examples we are going to use factories to create
 * the messages, but could be used any other kind of constructible
 * strategy (see 'serialize_parse_reliable' example).
 *
 * \note (https://tools.ietf.org/html/rfc8323#section-5)
 */

#include <cstdint>

#include "coap-te/log.hpp"				//Log
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp" 	//Convenient debug header

/**
 * Buffer length
 */
#define BUFFER_LEN		512

using namespace CoAP::Log;
using namespace CoAP::Message;

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
	debug("Reliable Factory Signaling example...");

	CoAP::Error ec;

	{
		/**
		 * Making a CSM signaling message
		 */
		status("--------------");
		status("Making a CSM signaling message...");

		/**
		 * Instantiating the factory (with internal buffer)
		 */
		Reliable::Factory<512, code::csm> fac;

		/**
		 * Creating CSM options
		 */
		unsigned max_size = 4048;
		Option::node_csm max_size_op{Option::csm::max_message_size, max_size};
		Option::node_csm bl_wise_op{Option::csm::block_wise_transfer};

		/**
		 * Adding options to the factory and serilizing
		 * The signal code CSM 7.01 is automatically added
		 */
		std::size_t size = fac
							.add_option(max_size_op)
							.add_option(bl_wise_op)
							.serialize(ec);

		/**
		 * Checking if serialization was OK
		 */
		if(ec) exit_error(ec, "serialize");
		status("Serialized CSM message success! [%zu]", size);
		/**
		 * Printing buffer
		 */
		CoAP::Debug::print_byte_reliable_message(fac.buffer(), size);
		debug("---------------------");
		status("Parsing CSM message...");

		/**
		 * Parsing message
		 */
		Reliable::message msg;
		auto size_parse = Reliable::parse(msg, fac.buffer(), size, ec);

		/**
		 * Checking if parse was OK
		 */
		if(ec) exit_error(ec, "parsing");

		/**
		 * Printing parsed message
		 */
		status("Message parsed successfully! [%u]", size_parse);
		CoAP::Debug::print_message(msg);
	}

	{
		/**
		 * Making a Ping/Pong signaling message
		 */
		status("--------------");
		status("Making a Ping/Pong signaling message...");

		/**
		 * Instantiating the factory (with internal buffer)
		 * To a pong message, use code::pong
		 */
		Reliable::Factory<512, code::ping> fac;

		/**
		 * Creating Ping/Pong options
		 * Option::node_ping and Option::node_pong are the same thing
		 */
		Option::node_ping custody_op{Option::ping_pong::custody};

		/**
		 * Adding and serializing
		 * The signal code Ping 7.02 is automatically added
		 */
		std::size_t size = fac
							.add_option(custody_op)
							.serialize(ec);
		/**
		 * Checking if serialization was OK
		 */
		if(ec) exit_error(ec, "serialize");
		status("Serialized Ping/Pong message success! [%zu]", size);
		/**
		 * Printing buffer
		 */
		CoAP::Debug::print_byte_reliable_message(fac.buffer(), size);
		debug("---------------------");
		status("Parsing Ping/Pong message...");

		/**
		 * Parsing message
		 */
		Reliable::message msg;
		auto size_parse = Reliable::parse(msg, fac.buffer(), size, ec);

		/**
		 * Checking if parse was OK
		 */
		if(ec) exit_error(ec, "parsing");

		/**
		 * Printing parsed message
		 */
		status("Message parsed successfully! [%u]", size_parse);
		CoAP::Debug::print_message(msg);
	}

	{
		status("--------------");
		status("Making a Release signaling message...");

		/**
		 * Instantiating the factory (with internal buffer)
		 */
		Reliable::Factory<512, code::release> fac;

		/**
		 * Creating Release options
		 */
		Option::node_release addr1_op{Option::release::alternative_address, "some_address"};
		Option::node_release addr2_op{Option::release::alternative_address, "other_address"};
		unsigned hold_seconds = 10;
		Option::node_release holdoff_op{Option::release::hold_off, hold_seconds};

		/**
		 * Adding and serializing
		 * The signal code Release 7.04 is automatically added
		 */
		std::size_t size = fac
							.add_option(addr1_op)
							.add_option(addr2_op)
							.add_option(holdoff_op)
							.serialize(ec);
		/**
		 * Checking if serialization was OK
		 */
		if(ec) exit_error(ec, "serialize");

		/**
		 * Printing buffer
		 */
		status("Serialized Release message success! [%zu]", size);
		CoAP::Debug::print_byte_reliable_message(fac.buffer(), size);
		debug("---------------------");
		status("Parsing Release message...");

		/**
		 * Parsing message
		 */
		Reliable::message msg;
		auto size_parse = Reliable::parse(msg, fac.buffer(), size, ec);

		/**
		 * Checking if parse was OK
		 */
		if(ec) exit_error(ec, "parsing");

		/**
		 * Printing parsed message
		 */
		status("Message parsed successfully! [%u]", size_parse);
		CoAP::Debug::print_message(msg);
	}

	{
		status("--------------");
		status("Making a Abort signaling message...");

		/**
		 * Instantiating the factory (with internal buffer)
		 */
		Reliable::Factory<512, code::abort> fac;

		/**
		 * Creating Abort options
		 */
		unsigned op = 2;
		Option::node_abort bad_csm_op{Option::abort::bad_csm_option, op};

		/**
		 * Adding and serializing
		 * The signal code Abort 7.05 is automatically added
		 */
		std::size_t size = fac
							.add_option(bad_csm_op)
							.serialize(ec);
		/**
		 * Checking if serialization was OK
		 */
		if(ec) exit_error(ec, "serialize");

		/**
		 * Printing buffer
		 */
		status("Serialized Abort message success! [%zu]", size);
		CoAP::Debug::print_byte_reliable_message(fac.buffer(), size);
		debug("---------------------");
		status("Parsing Abort message...");

		/**
		 * Parsing message
		 */
		Reliable::message msg;
		auto size_parse = Reliable::parse(msg, fac.buffer(), size, ec);

		/**
		 * Checking if parse was OK
		 */
		if(ec) exit_error(ec, "parsing");

		/**
		 * Printing parsed message
		 */
		status("Message parsed successfully! [%u]", size_parse);
		CoAP::Debug::print_message(msg);
	}

	return EXIT_SUCCESS;
}

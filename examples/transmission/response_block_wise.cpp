/**
 * This example is a CoAP server shows how to use block wise
 * CoAP transfer (RFC 7959).
 *
 * Making a request to "\/data" resource with:
 * * GET method: will transfer some large amount of data from the
 * server to the requested client.
 * * PUT method: all data received from the client will be copied
 * to a buffer and then printed.
 *
 * Use this example together with "request_get_block_wise" and
 * "request_put_block_wise".
 */

#include <cstdio>

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

using namespace CoAP::Log;

#define COAP_PORT		CoAP::default_port		//5683
#define BUFFER_LEN		1024					//Buffer size
#define HOST_ADDR		"127.0.0.1"				//Address
#define TRANSACT_NUM	4

#define DEFAULT_BLOCK_SIZE	128					//Size of block defined to the server
#define RECEIVE_PUT_BUFFER	2048				//Receiving buffer size (for PUT method)

/**
 * Text copied from https://www.planetary.org/worlds/pale-blue-dot
 *
 * The GET method will transfer this data from to server to the client
 */
static constexpr const char huge_data[] = R"foo(
Look again at that dot. That's here. That's home. That's us. On it everyone you love, everyone you know, everyone you ever heard of, every human being who ever was, lived out their lives. The aggregate of our joy and suffering, thousands of confident religions, ideologies, and economic doctrines, every hunter and forager, every hero and coward, every creator and destroyer of civilization, every king and peasant, every young couple in love, every mother and father, hopeful child, inventor and explorer, every teacher of morals, every corrupt politician, every "superstar," every "supreme leader," every saint and sinner in the history of our species lived there--on a mote of dust suspended in a sunbeam.

The Earth is a very small stage in a vast cosmic arena. Think of the rivers of blood spilled by all those generals and emperors so that, in glory and triumph, they could become the momentary masters of a fraction of a dot. Think of the endless cruelties visited by the inhabitants of one corner of this pixel on the scarcely distinguishable inhabitants of some other corner, how frequent their misunderstandings, how eager they are to kill one another, how fervent their hatreds.

Our posturings, our imagined self-importance, the delusion that we have some privileged position in the Universe, are challenged by this point of pale light. Our planet is a lonely speck in the great enveloping cosmic dark. In our obscurity, in all this vastness, there is no hint that help will come from elsewhere to save us from ourselves.

The Earth is the only world known so far to harbor life. There is nowhere else, at least in the near future, to which our species could migrate. Visit, yes. Settle, not yet. Like it or not, for the moment the Earth is where we make our stand.

It has been said that astronomy is a humbling and character-building experience. There is perhaps no better demonstration of the folly of human conceits than this distant image of our tiny world. To me, it underscores our responsibility to deal more kindly with one another, and to preserve and cherish the pale blue dot, the only home we've ever known.

— Carl Sagan, Pale Blue Dot, 1994

Copyright © 1994 by Carl Sagan, Copyright © 2006 by Democritus Properties, LLC.
All rights reserved including the rights of reproduction in whole or in part in any form.
)foo";

/**
 * Buffer to receive data sent to the server, by a put method request
 */
static char recv_buffer[RECEIVE_PUT_BUFFER];

/**
 * Example log module
 */
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
};

/**
 * Engine definition. Check 'raw_engine' example for a full
 * description of the options.
 *
 * Here we are using server profile, and defining a resource callback
 * function. Server profile allow to add resources. We are also disabling
 * the default callback.
 */
using engine = CoAP::Transmission::engine<
		CoAP::Port::POSIX::udp<CoAP::Port::POSIX::endpoint_ipv4>,
		CoAP::Message::message_id,
		CoAP::Transmission::transaction_list<
			CoAP::Transmission::transaction<
				BUFFER_LEN,
				CoAP::Transmission::transaction_cb,
				CoAP::Port::POSIX::endpoint_ipv4>,
			TRANSACT_NUM>,
		void*,		//default callback disabled
		CoAP::Resource::resource<
			CoAP::Resource::callback<CoAP::Port::POSIX::endpoint_ipv4>,
			false
		>
	>;

/**
 * Response callback declaration
 *
 * Detailed explanation of the callbacks below 'main'
 */
static void get_data_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void put_data_handler(engine::message const& request,
								engine::response& response, void*) noexcept;

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
	/**
	 * At Linux, does nothing. At Windows initiate winsock
	 */
	CoAP::Port::POSIX::init();
	
	CoAP::Error ec;
	/**
	 * Socket
	 */
	//Initiating the endpoint to bind
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	engine::connection socket;

	socket.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");
	socket.bind(ep, ec);
	if(ec) exit_error(ec, "Error trying to bind socket...");

	debug(example_mod, "Socket opened and binded...");

	debug(example_mod, "Initiating server engine...");
	/**
	 * Initiating CoAP engine
	 */
	engine coap_engine(std::move(socket),
			CoAP::Message::message_id((unsigned)CoAP::time()));

	/**
	 * Resource instantiation
	 *
	 * Each resource must provide a path name, and a callback function to
	 * the method it support. (GET/POST/PUT/DELETE)
	 */
	engine::resource_node	res_data{"data", get_data_handler, nullptr, put_data_handler};

	debug(example_mod, "Adding resources... [%u]", sizeof(huge_data));
	/**
	 * Adding resource to the tree
	 */
	coap_engine.root_node().add_child(res_data);

	debug(example_mod, "Initiating CoAP engine loop...");
	//CoAP engine loop.
	while(coap_engine(ec));

	return EXIT_SUCCESS;
}

/**
 * GET method for \/data
 *
 * This handler will check the 'block2' option of the request, and respond
 * to the client accordingly
 */
static void get_data_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	using namespace CoAP::Message;

	Option::option b2;

	/**
	 * Default block2 options, if none is sent by the client
	 */
	unsigned block_size = DEFAULT_BLOCK_SIZE,
			block_num = 0,
			offset = 0,
			left = sizeof(huge_data);
	bool more = true;

	/**
	 * Checking if block2 option is present
	 */
	if(Option::get_option(request, b2, Option::code::block2))
	{
		unsigned value = Option::parse_unsigned(b2);

		block_size = Option::block_size(value);
		block_num = Option::block_number(value);
		offset = Option::byte_offset(value);

		/**
		 * Checking if block required is valid
		 */
		if(offset > sizeof(huge_data))
		{
			error(example_mod, "Invalid block requested");
			response
				.code(code::bad_request)
				.payload("out of bound")
				.serialize();
			return;
		}
		left -= offset;
		/**
		 * Setting more flag
		 */
		if(left < block_size)
			more = false;
	}

	/**
	 * Option content format
	 */
	content_format content{content_format::text_plain};
	Option::node content_op{content};

	/**
	 * Option size2
	 */
	unsigned size2 = sizeof(huge_data);
	Option::node size2_op{Option::code::size2, size2};

	/**
	 * Option block2
	 */
	unsigned block2;
	Option::make_block(block2, block_num, more, block_size);
	Option::node block2_op{Option::code::block2, block2};

	/**
	 * Sending response
	 */
	response
		.code(code::content)
		.add_option(content_op)
		.add_option(size2_op)
		.add_option(block2_op)
		.payload(huge_data + offset, left > block_size ? block_size : left)
		.serialize();
}

/**
 * PUT method for \/data
 *
 * This handler will check the 'block1' option of the request, and copy the data
 * sent to a local buffer. It will respond to the client accordingly and wait
 * for the next data to be sent. At end, prints all data received
 */
static void put_data_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	using namespace CoAP::Message;

	status(example_mod, "Received put data request");
	CoAP::Debug::print_message_string(request);

	Option::option opt;

	/**
	 * Checking size of data to be transfered
	 */
	if(Option::get_option(request, opt, Option::code::size1))
	{
		unsigned size = Option::parse_unsigned(opt);
		if(size > RECEIVE_PUT_BUFFER)
		{
			/**
			 * If data to be sent is too big, respond with error
			 */
			error(example_mod, "Data to receive bigger than buffer! Interrupting transfer!");
			response
				.code(code::request_entity_too_large)
				.serialize();
			return;
		}
	}

	/**
	 * Checking if is a block wise transfer
	 */
	if(Option::get_option(request, opt, Option::code::block1))
	{
		unsigned value = Option::parse_unsigned(opt);

		unsigned block_size = Option::block_size(value),
				block_num = Option::block_number(value),
				offset = Option::byte_offset(value);
		bool more = Option::more(value);
		/**
		 * Checking if block required is valid
		 */
		if(offset > RECEIVE_PUT_BUFFER)
		{
			error(example_mod, "Message to big");
			response
				.code(code::request_entity_too_large)
				.payload("out of bound")
				.serialize();
			return;
		}
		/**
		 * Copying received data to buffer
		 */
		status(example_mod, "Copying to buffer [%lu]", request.payload_len);
		std::memcpy(recv_buffer + offset, request.payload, request.payload_len);

		/**
		 * Is there more blocks to be sent?
		 */
		if(!more)
		{
			/**
			 * If no more blocks to receive, just print result
			 */
			status(example_mod, "Data received:\n----------------------\n");
			std::printf("%s", recv_buffer);
			status(example_mod, "---------------------\nAll data transfered!\n\n");
		}

		/**
		 * Making block option
		 */
		unsigned block1;
		Option::make_block(block1,
							block_num,
							more,
							block_size < DEFAULT_BLOCK_SIZE ? block_size : DEFAULT_BLOCK_SIZE);	//Choosing smaller block size
		Option::node block1_op{Option::code::block1, block1};

		response
			.code(more ? code::ccontinue : code::changed)
			.add_option(block1_op)
			.serialize();
		return;
	}

	/**
	 * Not block wise transfer
	 */
	std::memcpy(recv_buffer, request.payload, request.payload_len);
	/**
	 * Printing received data
	 */
	status(example_mod, "Data received:\n----------------------\n");
	std::printf("%s", recv_buffer);
	status(example_mod, "---------------------\nAll data transfered!\n\n");

	response
		.code(code::changed)
		.serialize();
}


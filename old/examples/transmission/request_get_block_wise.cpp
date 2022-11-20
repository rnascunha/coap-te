/**
 * This example will show how to use 'block2' option to receive
 * big chunks of data from the server to the client, using
 * CoAP block wise transfer (RFC7959).
 */

#include <cstring>

#include "coap-te/log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

using namespace CoAP::Log;

#define COAP_PORT		CoAP::default_port		//5683
#define HOST_ADDR		"127.0.0.1"				//Address

#define BUFFER_LEN			4048		//Local buffer size
#define TRANSC_BUFFER_LEN	512			//Transaction buffer length
#define TRANSFER_BLOCK_SIZE	64			//Block size

#define RESOURCE_DATA_PATH	"data"		//resource which will make a request

/**
 * Log module
 */
static constexpr module example_mod = {
		/*.name = */"EXAMPLE",
		/*.max_level = */CoAP::Log::type::debug
};

/**
 * Transmission configuration settings
 */
static constexpr const CoAP::Transmission::configure tconfigure = {
	/*.ack_timeout_seconds 			= */2.0,	//ACK_TIMEOUT
	/*.ack_random_factor 			= */1.5,	//ACK_RANDOM_FACTOR
	/*.max_restransmission 			= */4,		//MAX_RETRANSMIT
};

/**
 * Engine definition. Check 'raw_engine' example for a full
 * description os the options.
 */
using engine = CoAP::Transmission::engine<
		CoAP::Port::POSIX::udp<CoAP::Port::POSIX::endpoint_ipv4>,
		CoAP::Message::message_id,
		CoAP::Transmission::transaction_list<
			CoAP::Transmission::transaction<
					TRANSC_BUFFER_LEN,
					CoAP::Transmission::transaction_cb,
					CoAP::Port::POSIX::endpoint_ipv4>,
				4>,
		CoAP::Transmission::default_cb<
			CoAP::Port::POSIX::endpoint_ipv4>,
			CoAP::disable
	>;

/**
 *
 *
 */

/**
 * Auxiliary function
 */
void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

//Response flag to check if response was received
static bool response_flag = false;
/**
 * Copy data received to buffer
 */
static char receive_data[BUFFER_LEN];

/**
 * Request callback (signature defined at transaction)
 */
void request_cb(void const* trans, engine::message const* response, void* eng_ptr) noexcept
{
	debug(example_mod, "Request callback called...");

	/**
	 * Checking response
	 */
	if(response)
	{
		status(example_mod, "Response received!");
		CoAP::Debug::print_message_string(*response);

		using namespace CoAP::Message;

		/**
		 * The size2 option, if present, informs the total size of the
		 * data that will be transfered
		 */
		Option::option opt;
		if(Option::get_option(*response, opt, Option::code::size2))
		{
			/**
			 * Checking if the size fits our buffer
			 */
			unsigned size = Option::parse_unsigned(opt);
			if(size > BUFFER_LEN)
			{
				error(example_mod, "Data size to bigger than buffer! Interrupting transfer\n");
				response_flag = true;
				return;
			}
		}

		if(Option::get_option(*response, opt, Option::code::block2))
		{
			unsigned value = Option::parse_unsigned(opt);
			unsigned //block_size = Option::block_size(value),
					//block_num = Option::block_number(value),
					offset = Option::byte_offset(value);

			/**
			 * Copying received data to buffer
			 */
			std::memcpy(receive_data + offset, response->payload, response->payload_len);

			/**
			 * If no more data to be received, print data and return
			 */
			if(!Option::more(value))
			{
				std::printf("Data received:\n----------------------\n");
				std::printf("%s", receive_data);
				std::printf("---------------------\nAll data transfered!\n\n");
				response_flag = true;
				return;
			}

			/**
			 * Making next request block option
			 *
			 * The last parameter must be a value from 16 to 1024, power of 2,
			 * i.e: 16, 32, 64, 128, 256, 512 or 1024.
			 *
			 * Any other value will return false
			 */
			unsigned n_value;
			if(!Option::make_block(n_value, Option::block_number(value) + 1, 0, TRANSFER_BLOCK_SIZE))
			{
				error(example_mod, "Error making block\n");
				response_flag = true;
				return;
			}

			/**
			 * Making a request to get the next block.
			 */
			engine::transaction_t const* transac = static_cast<engine::transaction_t const*>(trans);
			Option::node block2_op{Option::code::block2, n_value};
			CoAP::Message::Option::node path_op{CoAP::Message::Option::code::uri_path, RESOURCE_DATA_PATH};

			/**
			 * Making request to ask for next block
			 */
			engine::endpoint ep{transac->endpoint()};
			engine::request req{ep};
			req
				.header(CoAP::Message::type::confirmable, code::get)
				.token(response->token, response->token_len)
				.add_option(block2_op)
				.add_option(path_op)
				.callback(request_cb, eng_ptr);
			CoAP::Error ec;

			/**
			 * Sending
			 */
			engine* eng = static_cast<engine*>(eng_ptr);
			eng->send(req, ec);
			return;
		}
		else
		{
			/**
			 * If not block wise transfer, just print the data
			 */
			status(example_mod, "Response doesn't have op Block2\n");

			std::memcpy(receive_data, response->payload, response->payload_len);
			std::printf("Data received[%zu]:\n\n", response->payload_len);
			std::printf("%.*s", static_cast<int>(response->payload_len),
								static_cast<char const*>(response->payload));
			std::printf("\nAll data transfered!\n\n");
		}
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		status(example_mod, "Response NOT received");
	}
	/**
	 * Setting flag that response was received
	 */
	response_flag = true;
}

int main()
{
	debug(example_mod, "Init engine code...");
	
	/**
	 * Window/Linux: Initialize random number generator
	 * Windows: initialize winsock library
	 */
	CoAP::init();


	CoAP::Error ec;

	/**
	 * Socket
	 */
	engine::connection conn;

	conn.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");

	debug(example_mod, "Socket opened...");

	debug(example_mod, "Initating the engine...");

	engine coap_engine(std::move(conn),
			CoAP::Message::message_id((unsigned)CoAP::time()),
			tconfigure);

	debug(example_mod, "Constructing the request message...");

	/**
	 * Making block option to request first block
	 *
	 * (the first block is not really necessary to send the block option,
	 * but if you know that is a block wise transfer, you can inform the
	 * block size that you are willing to deal.)
	 */
	unsigned block2;
	CoAP::Message::Option::make_block(block2, 0, 0, TRANSFER_BLOCK_SIZE);
	CoAP::Message::Option::node path_op{CoAP::Message::Option::code::uri_path, RESOURCE_DATA_PATH};
	CoAP::Message::Option::node block2_op{CoAP::Message::Option::code::block2, block2};

	/**
	 * Endpoint that request will be sent
	 */
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	/**
	 * Making request
	 */
	engine::request request(ep);
	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
			.token("token")
			.add_option(path_op)
			.add_option(block2_op)
			.callback(request_cb, &coap_engine);

	debug(example_mod, "Sending the request...");

	coap_engine.send(request, ec);
	if(ec) exit_error(ec, "send");

	debug(example_mod, "Message sended");

	debug(example_mod, "Initiating engine loop");

	/**
	 * Work loop
	 */
	while(!response_flag && coap_engine(ec));
	if(ec) exit_error(ec, "run");

	return EXIT_SUCCESS;
}

/**
 * This example will show how to use 'block1' option to send
 * big chunks of data from the client to a server, using
 * CoAP block wise transfer (RFC7959).
 */
#include <cstring>

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

using namespace CoAP::Log;

#define COAP_PORT		CoAP::default_port		//5683
#define HOST_ADDR		"127.0.0.1"				//Address

#define TRANSC_BUFFER_LEN	512					//Transaction buffer length
#define TRANSFER_BLOCK_SIZE	64					//Transfer block size

#define RESOURCE_DATA_PATH	"data"

/**
 * Data to be sent
 */
static constexpr const char data_send[] = R"(
Ridiculus proin faucibus id, eleifend ante, massa lacinia augue aptent class et mattis cubilia etiam nostra. Elit varius nonummy velit Parturient ultricies inceptos, enim, tempus. Phasellus metus cras fringilla parturient. Aliquet fusce magna inceptos ullamcorper. Neque nam lectus feugiat laoreet quis mattis velit gravida dolor primis turpis eu. Auctor. Sollicitudin venenatis morbi facilisi dignissim tincidunt hymenaeos. Montes vulputate. Lacus duis semper, dictum purus cum porttitor pellentesque vivamus. Libero.

Facilisi. Metus maecenas. Faucibus posuere conubia risus tristique eros pede turpis justo eleifend Vel non viverra purus tempus metus aenean. Fermentum dolor luctus arcu ullamcorper mollis arcu euismod primis vel, malesuada laoreet blandit donec montes varius quis mollis class vel amet nunc habitant inceptos quisque eget dictumst aptent magnis urna per mollis cras turpis ultricies velit.

Tempus nibh primis pede eget risus. Lacus metus rhoncus mollis non nonummy pharetra donec commodo curae;. Suscipit, arcu. A posuere. Sagittis velit nostra mollis. Venenatis mauris varius eleifend etiam pede commodo aenean, habitant in diam integer. Urna eros euismod tellus etiam a ultricies accumsan. Lorem montes fusce vivamus ligula. Vitae magna fusce viverra. Cum habitant.

Non turpis sem sagittis, lacinia auctor. Conubia molestie inceptos. Sapien erat libero. Adipiscing phasellus proin nec. Nisi, praesent dictum tristique in etiam sem In rhoncus consectetuer ante potenti sem neque. Semper ipsum sociosqu conubia, porta. Hymenaeos lobortis. Vestibulum aliquet libero, vestibulum primis rutrum varius luctus elementum eget phasellus lacus laoreet vel montes nibh senectus vivamus ante dui.
)";

/**
 * Log module
 */
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
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
		 * Check if a error was sent by the server
		 */
		if(is_error(response->mcode))
		{
			error("error code received! Interrupting transfer! [%s]", CoAP::Debug::code_string(response->mcode));
			response_flag = true;
			return;
		}

		/**
		 * Checking if server sent block1 option
		 */
		Option::option opt;
		if(Option::get_option(*response, opt, Option::code::block1))
		{
			unsigned value = Option::parse_unsigned(opt);

			/**
			 * Getting block wise values
			 */
			unsigned block_size = Option::block_size(value),
					block_num = Option::block_number(value),
					offset = Option::byte_offset(value);
			bool more = true;

			/**
			 * Using the smaller block size
			 */
			if(block_size > TRANSFER_BLOCK_SIZE)
				block_size = TRANSFER_BLOCK_SIZE;

			/**
			 * All data was sent?
			 */
			if(offset > sizeof(data_send))
			{
				/**
				 * All data was sent!
				 */
				status(example_mod, "All data sent!");
				response_flag = true;
				return;
			}

			/**
			 * Checking the left data to send and set if is the
			 * last block
			 */
			unsigned left = sizeof(data_send) - offset;
			if(left < block_size)
				more = false;

			/**
			 * Making next request block option
			 *
			 * The last parameter must be a value from 16 to 1024, power of 2,
			 * i.e: 16, 32, 64, 128, 256, 512 or 1024.
			 *
			 * Any other value will return false
			 */
			unsigned block1;
			if(!Option::make_block(block1, block_num + 1, more, block_size))
			{
				error(example_mod, "Error making block\n");
				response_flag = true;
				return;
			}

			/**
			 * Making a request to send the next block.
			 */
			Option::node block1_op{Option::code::block1, block1};
			Option::node path_op{Option::code::uri_path, RESOURCE_DATA_PATH};

			/**
			 * Sending the full size of data
			 */
			unsigned size1 = sizeof(data_send);
			Option::node size1_op{Option::code::size1, size1};

			engine::transaction_t const* transac = static_cast<engine::transaction_t const*>(trans);
			engine::endpoint ep{transac->endpoint()};

			/**
			 * Making the request
			 */
			engine::request req{ep};
			req
				.header(CoAP::Message::type::confirmable, code::put)
				.token(response->token, response->token_len)
				.add_option(block1_op)
				.add_option(path_op)
				.add_option(size1_op)
				.payload(data_send + offset, more ? block_size : left)
				.callback(request_cb, eng_ptr);

			/**
			 * Sending
			 */
			CoAP::Error ec;
			engine* eng = static_cast<engine*>(eng_ptr);
			eng->send(req, ec);
			return;
		}
		else
		{
			error(example_mod, "The server should have answer with a 'block1' so we could send the next block");
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
	 * At Linux, does nothing. At Windows initiate winsock
	 */
	CoAP::Port::POSIX::init();

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
						CoAP::Message::message_id(CoAP::time()));

	debug(example_mod, "Constructing the request message...");

	/**
	 * Making block1 request to send data
	 */
	unsigned block1;
	CoAP::Message::Option::make_block(block1, 0, true, TRANSFER_BLOCK_SIZE);
	CoAP::Message::Option::node path_op{CoAP::Message::Option::code::uri_path, RESOURCE_DATA_PATH};
	CoAP::Message::Option::node block1_op{CoAP::Message::Option::code::block1, block1};

	/**
	 * Endpoint to whon we are going to send
	 */
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	/**
	 * Making the request
	 */
	engine::request request(ep);
	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::put)
			.token("token")
			.add_option(path_op)
			.add_option(block1_op)
			.payload(data_send, TRANSFER_BLOCK_SIZE)
			.callback(request_cb, &coap_engine);

	debug(example_mod, "Sending the request...");

	/**
	 * Sending first request with the first block
	 */
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

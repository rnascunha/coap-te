/**
 * This example we are going to show how to use a transaction.
 *
 * Transaction are no meant to be used directly, but through
 * CoAP::Transmission::engine. But if you need to use at a more
 * complex environment, develop your own or just curious about
 * the CoAP-te internals, keep reading.
 *
 * To have a successful response to the transaction, you must have
 * a CoAP server running. Otherwise transaction will timeout.
 */

#include "log.hpp"			//Log header
#include "coap-te.hpp"		//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

using namespace CoAP::Log;
using namespace CoAP::Message;
using namespace CoAP::Transmission;

#define HOST_ADDR		"127.0.0.1"		//Address to connect
#define COAP_PORT		5683			//Port to connect
#define BUFFER_LEN		1024			//Buffer size
/**
 * Transactions can be used with internal buffers, or point to
 * a external one.
 *
 * Comment the following line to use with external buffer
 */
//#define USE_INTERNAL_BUFFER

/**
 * Transmission configuration settings
 * https://tools.ietf.org/html/rfc7252#section-4.8
 */
static constexpr const configure tconfigure = {
	.ack_timeout_seconds 			= 2.0,	//ACK_TIMEOUT
	.ack_ramdom_factor 				= 1.5,	//ACK_RANDOM_FACTOR
	.max_restransmission 			= 4,	//MAX_RETRANSMIT
};

/**
 * Flag to check response
 */
static bool response_flag = false;

//Log module definition
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
};

/**
 * Auxiliary function
 */
void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

/**
 * Here we are going to define our transaction. You must provide
 * 3 template parameter:
 *
 * * MaxPacketSize: if == 0, you must point to a external buffer when
 * initialize a transaction, otherwise is the internal buffer size. The size
 * is for ALL the CoAP message (not just the payload):
 * header (4 bytes) + token (0-8 bytes) + options(undefined) + payload
 *
 * * Transaction callback: the callback function that will be called
 * when transaction finish (successfully or not). The callback signature is:
 *
 * void(void const* transaction, CoAP::Message::message const* response, void* data)
 * = CoAP::Transmission::transaction_cb
 *
 * The arguments are: the transaction itself, the received response (if any or null),
 * and a user defined data. You could also use something like:
 * std::functional<void(void const*, CoAP::Message::message const*, void*)> to bind other
 * values, or use lambda function.
 *
 * * Endpoint: the endpoint of the sockets we are connecting.
 */
#ifdef USE_INTERNAL_BUFFER
using transaction_t = transaction<BUFFER_LEN,					//max packet size != 0
							CoAP::Transmission::transaction_cb,	//Default callback signature
							CoAP::socket::endpoint>;			//Socket endpoint
#else
using transaction_t = transaction<0,							//max packet size == 0 (use external buffer)
							CoAP::Transmission::transaction_cb,	//Default callback signature
							CoAP::socket::endpoint>;			//Socket endpoint
#endif

/**
 * Call
 */
void request_cb(void const* trans, CoAP::Message::message const* response, void* /* engine */) noexcept
{
	transaction_t const* t = static_cast<transaction_t const*>(trans);

	/**
	 * Check status_t at transmission/types.hpp the examine all transaction status
	 * and there meanings.
	 */
	status(example_mod, "Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	/**
	 * Could be checked (and been more precise) checking the transaction status
	 */
	if(response)
	{
		status(example_mod, "Response received!");
		CoAP::Debug::print_message(*response);
	}
	else
	{
		status(example_mod, "Response not received");
	}
	/**
	 * Asserting that a response was received
	 */
	response_flag = true;
}

int main()
{
	debug(example_mod, "Init transaction code...");

	CoAP::Error ec;

	/**
	 * Socket
	 */
	CoAP::socket conn;

	conn.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");

	debug(example_mod, "Socket opened...");

	CoAP::socket::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	status(example_mod, "Constructing message...");
	/**
	 * Creating object
	 * All object lifetime if of user responsability. Until "serialize" method
	 * been called, nothing is copied.
	 */
	Option::node path_op1{Option::code::uri_path, ".well-known"};
	Option::node path_op2{Option::code::uri_path, "core"};

	message_id mid{static_cast<unsigned>(CoAP::time())};

	/**
	 * Transaction are only used with confirmable messages. It holds the
	 * values sent, and retries to sent if doesn't have a answer, and calls
	 * the callback function when finishes (successfully or not)
	 */
	CoAP::Message::Factory<0> fac;
	fac.header(CoAP::Message::type::confirmable, code::get)
		.add_option(path_op1)
		.add_option(path_op2);

	/**
	 * All this code below are done internally by the CoAP::engine...
	 * But here, we are doing manually.
	 */
#ifdef USE_INTERNAL_BUFFER
	debug(example_mod, "Using transaction internal buffer...");
	transaction_t tr;

	/**
	 * Serializing the message to the internal buffer
	 */
	tr.serialize(fac, mid(), ec);
	if(ec) exit_error(ec, "serialize");

	debug(example_mod, "Message to send [%d]",tr.buffer_used());
	CoAP::Debug::print_byte_message(tr.buffer(), tr.buffer_used());

	debug(example_mod, "Sending message...");
	//Sending
	conn.send(tr.buffer(), tr.buffer_used(), ep, ec);
	if(ec) exit_error(ec, "send");

	/**
	 * After successfully sending the message, we need to
	 * initiate the transaction. This mean to start all the timers
	 * to a possible retransmission. It also holds the endpoint,
	 * callback function and data until the transaction is finished
	 *
	 * The timers are calculated based at the first parameter 'tconfigure'.
	 */
	tr.init(tconfigure, ep, request_cb, nullptr, ec);
	if(ec) exit_error(ec, "init transaction");
#else
	debug(example_mod, "Using transaction external buffer...");
	/**
	 * External buffer to be used
	 */
	std::uint8_t buffer_send[BUFFER_LEN];
	transaction_t tr;

	/**
	 * We must serialize the buffer outside the transaction
	 */
	std::size_t size = fac.serialize(buffer_send, BUFFER_LEN, mid(), ec);
	if(ec) exit_error(ec, "serialize");

	debug(example_mod, "Message to send [%d]", size);
	CoAP::Debug::print_byte_message(buffer_send, size);

	debug(example_mod, "Sending data...");
	//Sending
	conn.send(buffer_send, size, ep, ec);
	if(ec) exit_error(ec, "send");

	/**
	 * After successfully sending the message, we need to
	 * initiate the transaction. This mean to start all the timers
	 * to a possible retransmission. It also holds the endpoint,
	 * callback function and data until the transaction is finished
	 *
	 * The timers are calculated based at the first parameter 'tconfigure'.
	 */
	tr.init(tconfigure, ep, buffer_send, size, request_cb, nullptr, ec);
	if(ec) exit_error(ec, "init transaction");
#endif /* USE_INTERNAL_BUFFER */

	/**
	 * Receiving message
	 */

	std::uint8_t buffer_recv[BUFFER_LEN];	//Receiving buffer;
	char print_buffer[20];					//auxiliary buffer to print

	CoAP::socket::endpoint ep_recv;			//endpoint that will hold response endpoint
	CoAP::Message::message response;		//Where the receiving message will be parsed
	std::size_t size_recv = 0;

	/**
	 * This loop will run until a successful message be received or
	 * the transaction timeout
	 */
	while(!response_flag)
	{
		//receiving
		size_recv = conn.receive(buffer_recv, BUFFER_LEN, ep_recv, ec);
		if(ec) exit_error(ec, "read");

		//If receive something
		if(size_recv)
		{
			status(example_mod, "From: %s:%u", ep_recv.host(print_buffer), ep_recv.port());
			CoAP::Message::parse(response, buffer_recv, size_recv, ec);
			if(ec) exit_error(ec, "parse");

			/**
			 * this will check if is the response from the transaction  (in a
			 * real life application, it could have many parallel transaction
			 * working). It will check the message_id and endpoint.
			 * You can also set to not check the endpoint, if you are at a controlled
			 * environment, or enable token check.
			 *
			 * Check transaction definition at 'transmission/transaction.hpp',
			 * method check_response template parameters.
			 *
			 * If the response was correctly received, it will call the callback
			 * defined at transaciton.init
			 */
			if(tr.check_response(ep_recv, response))
				status(example_mod, "Response received correctly");
		}
		else
		/**
		 * Here we check if the message timed out
		 *
		 * If it returns true, it means we need to retransmit the message. False
		 * could be any other thing. You could always check the transaction status.
		 */
		if(tr.check())
		{
			status(example_mod, "Message timeout");
#ifdef USE_INTERNAL_BUFFER
			conn.send(tr.buffer(), tr.buffer_used(), ep, ec);
#else
			conn.send(buffer_send, size, ep, ec);
#endif
			if(ec) exit_error(ec, "retransmit");
			tr.retransmit();
		}
	}
	return EXIT_SUCCESS;
}

# CoAP-te

**CoAP-te** is a C++17 implementation (work in progress) of CoAP protocol ([RFC7252](https://tools.ietf.org/html/rfc7252)).

The implemtation aims to:
* Don't use any *dynamic allocation* or *exceptions*;
* Configurable: the CoAP *engine* be configurable to your system/environment requirements;
* Modularized: As much as possible, all modules work independently;
* Portable: simple to port to your system/device.

It's implemented:
* CoAP ([RFC7252](https://tools.ietf.org/html/rfc7252));
* Block-wise transfer ([RFC7959](https://tools.ietf.org/html/rfc7959));
* Reliable connections ([RFC8323](https://tools.ietf.org/html/rfc8323));
* Resource Discovery ([RFC6690](https://tools.ietf.org/html/rfc6690));
* Observing ([RFC7641](https://tools.ietf.org/html/rfc7641));

It's NOT implemented (yet):
* Cache strategie;
* Security;
* Much more :(...

## Dependencies

As external library dependency, **CoAP-te** uses [Tree Trunks](https://github.com/rnascunha/tree_trunks) as log system (same author).

It also needs the following applications to build/compile:
* C++17 compiler;
* [git](https://git-scm.com/) to download;
* [CMake](https://cmake.org/) to build and compile;

## Download and Compile

To download **CoAP-te**:

```
$ git clone --recursive https://github.com/rnascunha/coap-te
# enter directory
$ cd coap-te
```
To build:

```
# create a build directory and enter
$ mkdir build
$ cd build

# build
$ cmake ..
$ make
```
This will generate a library file (`libcoap-te.a` at linux). Link this file to your project, and include `./src/` directory. There are two convenient headers:
* `coap-te.hpp`: this has all necessary headers to use **CoAP-te**;
* `coap-te-debug.hpp`: this includes some print functions to help debug messages.

If you also want to compile the examples (at `examples` directory), run at the build directory:

```
$ cmake -DWITH_EXAMPLES=1 ..
$ make
```

This will generate one excutable to each example.

## Using

If you build **CoAP-te** with the `-DWITH_EXAMPLES=1` flag, all example executables are at your build directory. The directory `examples` contains the source code of the examples with detailed exaplanations on how to use **CoAP-te**. A brief overview is show here:

*Messages* examples:
* `serialize_parse`: shows how to serialize message using 3 strategies (factory and manually using option list/array). Then parse this information (as it was received by network) and iterate through options.
* `factory`: demostrate how to use a factory to construct a message, using internal/external buffers.
* `option`: shows how to manipulate options of different types. How to declare, serialize and parse.
* `serialze_class`: explain the use of the `Serialize` class. This is a class to modify the information that were already serilized (written to the buffer).
* `serialize_parse_reliable`: the same of `serialize_parse` example, but for reliable connections (RFC8323).
* `signaling`: explain how to make/parse a signaling message to reliable connections (RFC8323). 

*Transmission* examples:
* `raw_transaction`: explains the use of transactions. Transactions are not meant to be used directly, but through the *CoAP::engine*.
* `raw_engine`: demonstrate how to configure and use your own engines, the central feature of **CoAP-te**. Engines
deal with all CoAP transmission complexity. After configuration, makes a simple CoAP request.
* `engine_server`: shows how to add resource to a engine, and how to make the response to specific methods, using different strategies.
* `request_get_block_wise`: this example makes a *GET* request using *block2* block wise transfer, from a client to a server. Use with `response_block_wise` example. 
* `request_put_block_wise`: this example makes a *PUT* request using *block1* block wise transfer, from a client to a server. Use with `response_block_wise` example.
* `response_block_wise`: this example is a server that responds to the `request_get_block_wise` and `request_put_block_wise` examples above.
* `engine_tcp_client`: demonstrate how configure and use a reliable (RFC8323) client engine.
* `engine_tcp_server`: demonstrate how configure and use a reliable (RFC8323) server engine.

*Observe* examples: observe resource as defined at RFC7641.
* `client_observe`: client CoAP making 3 request to observe 3 different resources (confirmable and non-confirmable messages). To be used with `server_observe` example. 
* `server_observe`: server CoAP that holds 3 resource that can be observed, sending confirmable or non-confirmable message. To be used with `client_observe` example.
* `tcp_client_observe`: the same of `client_observe` example, but using reliable connection (TCP), as defined at (RFC8323).
* `tcp_server_oberver`: the same of `server_observe` example, but using reliable connection (TCP), as defined at (RFC8323). 

*URI* examples:
* `decompose`: breaks a CoAP URI into internal structures of **CoAP-te**, ready to be used. Any percent-encoded characters is converted. 

*Internal* examples: this examples are to test internal functions used, not intended to the end user.
* `tree`: shows how to work with *trees*, the structure that holds all the resources.
* `percent_encode`: demonstrate the use of function `percent_encode` used to compose URI strings;
* `percent_decode`: demonstrate the use of function `percent_decode` used to decompose URI strings;

*Port* examples: here are show the raw use of connections/plataform dependent functions. Not intended to the end user.
* `udp_server`: the use of the implemented posix-like UDP socket. Run a echo UDP server;
* `udp_client`: the use of the implemented posix-like UDP socket. Run a UDP client making a request and wait for a response;
* `tcp_server`: the use of the implemented posix-like TCP socket (RFC8323). Run a echo TCP server;
* `tcp_client`: the use of the implemented posix-like TCP socket (RFC8323). Run a TCP client making a request and wait for a response;

## Portability

To port **CoAP-te** to your system, you must define:
* the following *free functions*:

```c++
namespace CoAP{
using time_t = <some_interger_type> 

/**
 * \brief Return time in epoch format (seconds)
 */
time_t time() noexcept;

/**
 * \brief Random number generator
 */
unsigned random_generator() noexcept;

}//CoAP
```
> The default implementation uses `std::time` and `std::rand` as the functions above, respectivily, and uses `time_t` as `std::time_t`. If your system support this functions, just use then.   

* *Endpoint*: endpoints must be *default constructable*, *copiable* and *comparable*;
* *Connection*: connection can be of type unreliable (*UDP*) or reliable (*TCP*/*Websocket*). A connection must define a *endpoint* (as shown above), and MUST not block. Each type of connection is discussed. 

> There is already a implementation to posix-like sockets that should be used as example. Check `src/port/posix` directory. You will see the implementation of endpoints IPv4 and IPv6, unreliable UDP sockets, and reliable TCP client and server sockets. The uses of this implementaions can be analyzed at the `examples/port` directory.

### Unreliable

To implement a unrealible connection type (as defined RFC7252) you must define a class as follows: 

```c++
struct my_connection{
	using endpoint = <some_endpoint>;

	std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
	std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
}
```
This class must be *moveable*. The receiving/send functions *MUST NOT block*. Any error calling the functions must be reported at the last parameter (*CoAP::errc::socket_error*). And... that's it folks.

### Reliable

Reliable connections (as defined RFC8323) must be separated in client (connect to one server) and server (many clients connects to it). Also, depending on the connection (TCP or Websocket) it also reflects the message format.

#### Client

It follows the example of a reliable client connection:

```c++
class my_client{
	public:
		static constexpr bool set_length = <message_format_style>;
		using handler = <some_handler_type>;
		using endpoint = <some_endpoint type>;
	
		handler native() const noexcept;

		void open(endpoint&, CoAP::Error&) noexcept;

		bool is_open() const noexcept;
		void close() noexcept;

		std::size_t send(const void*, std::size_t, CoAP::Error&)  noexcept;
		std::size_t receive(void*, std::size_t, CoAP::Error&) noexcept;
};
```

* `set_length`: tells the connection message format, i.e., if the message will have the length (first field) of the packet. *TCP* must be **true**, *Websocket* must be **false**;
* `handler`: handler that holds the socket (**int** to POSIX type);
* `endpoint`: endpoint type. Could be `endpoint_ipv4` or `endpoint_ipv6` to *TCP*/*Websocket*;
* `native` function: returns connection handler; 
* `open` function: connects to the server. This function can block until it connects. It must set the socket to non-block mode (if necessary);
* `is_open` function: check if the socket is opened;
* `close` function: closes the socket;
* `send` function: sends buffer to the server;
* `receive` function: receives data from the the server. It must return the buffer read size. If the connection is *TCP*, it can return all the buffer read. If it's *Websocket*, it must return packet by packet. It's very important this function **NOT BLOCK**.

#### Server

It follows the example of a reliable server connection:

```c++
class my_server{
	public:
		static constexpr bool set_length = <message_format_style>;
		using handler = <some_handler_type>;
		using endpoint = <some_endpoint type>;

		my_server();

		void open(endpoint&, CoAP::Error&) noexcept;
		bool is_open() const noexcept;

		template<
			int BlockTimeMs,
			unsigned MaxEvents,
			typename ReadCb,
			typename OpenCb,
			typename CloseCb>
		bool receive(void* buffer, std::size_t buffer_len, CoAP::Error&,
				ReadCb read_cb, OpenCb open_cb, CloseCb close_cb) noexcept;

		std::size_t send(handler, const void*, std::size_t, CoAP::Error&)  noexcept;

		void close() noexcept;
		void close_client(handler) noexcept;
}
```

* `set_length`: tells the connection message format, i.e., if the message will have the length (first field) of the packet. *TCP* must be **true**, *Websocket* must be **false**;
* `handler`: handler that holds the socket (**int** to POSIX type);
* `endpoint`: endpoint type. Could be `endpoint_ipv4` or `endpoint_ipv6` to *TCP*/*Websocket*;
* `open` function: open socket and bind endpoint server address;
* `is_open` function: check if the socket is opened;
* `close` function: closes the socket;
* `close_client` function: closes client socket;
* `send` function: sends buffer to the client;
* `receive` function: this function must: 
	* accept incoming connections: every incoming connection MUST call the `open_cb` function, with the handler passed at the argument; 
	* receives data from the the server: if the connection is *TCP*, call the `read_cb` function with the whole readed buffer. If its *Websocket*, it must call `read_cb` packet by packet;
	* check closed connection: call `close_cb` at any closed clients call, with the handler of the client;
	* any socket error must be reported at the `CoAP::Error&` argument.
 
> The template parameters `BlockTimeMs` and `MaxEvents` are passed to the `epoll_wait` call and may not have use at other envirioments. 

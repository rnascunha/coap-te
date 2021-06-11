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
* Fetch, Patch and iPatch methods ([RFC8132](https://tools.ietf.org/html/rfc8132));
* No response option ([RFC7967](https://tools.ietf.org/html/rfc7967));

It's NOT implemented (yet):
* Cache strategie;
* Security;
* Much more :(...

**CoAP-te** is ported to:
* Linux (GCC);
* Windows (MSVC);
* ESP32 (tested with [ESP-IDF 4.3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)).

Check the [Portability](#portability) section to see how to port **CoAP-te** to your device/environment.

## Dependencies

As external library dependency, **CoAP-te** uses [Tree Trunks](https://github.com/rnascunha/tree_trunks) as log system (same author).

It also needs the following applications to build/compile:
* C++17 compiler;
* [git](https://git-scm.com/) to download;
* [CMake](https://cmake.org/) to build and compile;

### Windows

Download and install the above software (tested with [Visual Studio](https://visualstudio.microsoft.com/) compiler).

### Linux

Debian-like systems:

```
#Download packages (libssl-dev only required for SSL support)
$ sudo apt-get install git cmake build-essential libssl-dev
```

Red Hat-like systems:

```
#Download packages (openssl-devel only required for SSL support)
$ sudo yum install git cmake make gcc gcc-c++ openssl-devel
```

## Download and Compile

### Linux / Windows

Let's clone **CoAP-te** repository:

```
$ git clone --recursive https://github.com/rnascunha/coap-te
# enter directory
$ cd coap-te
```
Build and compile:

```
# create a build directory and enter
$ mkdir build
$ cd build

# build
$ cmake ..
$ cmake --build .
```
This will generate a library file (`libcoap-te.a` at Linux, `coap-te.lib` at Windows). Link this file to your project, and include `./src/` directory. There are two convenient headers:
* `coap-te.hpp`: this has all necessary headers to use **CoAP-te**;
* `coap-te-debug.hpp`: this includes some print functions to help debug messages.

If you also want to compile the examples (at `examples` directory), run at the build directory:

```
$ cmake -DWITH_EXAMPLES=1 ..
$ cmake --build .
```

This will generate one excutable to each example.

### ESP32-IDF

First, you need to set your envirioment as explained [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).

The following instructions must be done to each project you intend to use **CoAP-te** with ESP32.

We are going to use **CoAP-te** as a component to your project. The [IDF build system](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) search for components at the `components` directory. If your project doesn't have one, create one at the root of the directory.

```
#enter project directory
$ cd <project_directory>
#create component directory and enter
$ mkdir components
$ cd components
```
Now we are going download **CoAP-te**:

```
$ git clone --recursive https://github.com/rnascunha/coap-te
```
If everything was done right, the ESP-IDF will see **CoAP-te** as a component. Now, at the main project, you must enable C++17 to compile your code. After register your project, set the `std=gnu++17` flag as shown below:

```
# Registering project
idf_component_register(SRCS /* project source files */
                       INCLUDE_DIRS /* include project dirs */
                       REQUIRES)

# Compile flags need by CoAP-te
target_compile_options(${COMPONENT_LIB} 
						PRIVATE -std=gnu++17)
```
Check the [ESP32 examples](#esp32). 

## Bulding Options

The following options can be passed to the `cmake` command to change compilation of **CoAP-te**.

```
$ cmake <options> ..
```
*General* options:
* `-DWITH_EXAMPLES=<0|1>`: enable (`1`) / disable (`0`, default) build examples;
* `-DLOG_COLOR=<0|1>`: enable (`1`, default) / disable (`0`) output log color (to posix-like terminal coloring);
* `-DLOG_LEVEL=<0-5>`: define log level output, where `0` disable log, and `5` log everything;
* `-DERROR_MESSAGE=<0|1>`: enable (`1`, default) / disable(`0`) human readable error messages (`Error.message()` call). Can disabled to save some bytes.

<!--
*WebAssembly (Emscripten)* specific:
* `-DWASM_OUTPUT_HTML=<0|1>`: defines WebAssembly examples output. `1`: HTML (*.html*, default), `0`: JavaScript (*.js*). All examples create also the compiled *.wasm* output.
* `-DWASM_EMRUN_ENABLE=<0|1>`: defines `--emrun` link flag to examples. `1`: enable flag (default), `0`: disable flag. More about [here](https://emscripten.org/docs/compiling/Running-html-files-with-emrun.html).
-->

> At **ESP32 plataform**, you can configure this parameters by at `menuconfig`, going to: `$ idf.py menuconfig` > `Component config` > `CoAP-te Configuration`. 

## Using

It follows a description of all the examples that explains how to use **CoAP-te**.

### Linux / Windows

If you build **CoAP-te** with the `-DWITH_EXAMPLES=1` flag, all example executables are at your build directory. The directory `examples` contains the source code of the examples with detailed exaplanations on how to use **CoAP-te**. A brief overview is show here:

*Messages* examples:
* `serialize_parse`: shows how to serialize message using 3 strategies (factory and manually using option list/array). Then parse this information (as it was received by network) and iterate through options.
* `factory`: demostrate how to use a factory to construct a message, using internal/external buffers.
* `option`: shows how to manipulate options of different types. How to declare, serialize and parse.
* `serialze_class`: explain the use of the `Serialize` class. This is a class to modify the information that were already serilized (written to the buffer).
* `serialize_parse_reliable`: the same of `serialize_parse` example, but for reliable connections (RFC8323).
* `signaling`: explain how to make/parse a signaling message to reliable connections (RFC8323);
* `no_response`: use of *no_response* option (RFC7967).

*Transmission* examples:
* `raw_transaction`: explains the use of transactions. Transactions are not meant to be used directly, but through the *CoAP::engine*.
* `raw_engine`: demonstrate how to configure and use your own engines, the central feature of **CoAP-te**. Engines deal with all CoAP transmission complexity. After configuration, makes a simple CoAP request.
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

### ESP32

All the examples shown for Linux/Windows system can also be used to understand how to use **CoAP-te** at ESP32. You just need to deal with the specificities of the microcontroller envirioment.

The ESP32 examples can be found at the `examples/esp32` directory. To use then, copy the directory and initialize the ESP-IDF envirioment as explained [here](#esp32-idf). After that, configure the examples, compile and flash it:

```
#Configure
$ idf.py menuconfig

#compile, flash and monitor
$ idf.py build flash monitor
```

Some important advise:
* Before use any examples, you MUST configure it using the `idf.py menuconfig` command. Go to the `Example WiFi/Server Configuration` and set all the options accordingly. Some examples have specific options, but at all you must set your WiFi parameters, as *SSID* and *password*.
* All examples use the function `wifi_stack_init`. This function will block until connect to the WiFi, or get stuck there if fail. This is a very naive implementaion, and should NOT be used at production.
* At the ESP32 examples, we prefered to instantiate the **CoAP-te** engine globally, to not be limited by the task memory size. If you prefer to use inside a task, depending on the *max packet size*/*transaction number*, it can overflow the task memory stack. Be aware of this. If you want to do this at the main task (`app_main`), you can increase the task memory stack if you need going to `idf.py menuconfig` > `Component config` > `Common ESP-related` > `Main task size`.

It follows a brief overview of the ESP32 examples.

* `engine_server`: demonstrate how to use a **CoAP-te** engine, using a UDP connection.
* `engine_tcp_server`: demonstrate how to use a **CoAP-te** engine, using a TCP connection.

*Port* examples: `tcp_client`, `tcp_server` and `udp_socket` are just some basic use of the socket ported to the ESP32. Not intended for the end user.

## Portability

To port **CoAP-te** to your system, you must define:
* the following *free functions*:

```c++
namespace CoAP{
using time_t = <some_interger_type>;

/**
 * \brief Return time in epoch format (milliseconds)
 */
time_t time() noexcept;

/**
 * \brief Random number generator
 */
unsigned random_generator() noexcept;

}//CoAP
```

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
			int BlockTimeMs = 0,
			unsigned MaxEvents = 32,
			typename ReadCb,
			typename OpenCb = void*,
			typename CloseCb = void*>
		bool run(CoAP::Error&,
				ReadCb, OpenCb, CloseCb) noexcept;

		std::size_t receive(handler socket, void* buffer, std::size_t, CoAP::Error&) noexcept;
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
* `send` function: sends data buffer to client;
* `receive` function: receive data from client;
* `run` function: this function must: 
	* accept incoming connections: every incoming connection MUST call the `open_cb` function, with the handler passed at the argument; 
	* check if there is data to receive from clients: if there is, call `read_cb` function with the client handler passed as argument;
	* check closed connection: call `close_cb` at any closed clients call, with the handler of the client;
	* any socket error must be reported at the `CoAP::Error&` argument.
 
> The template parameters `BlockTimeMs` and `MaxEvents` are passed to the `epoll_wait` call and may not have use at other envirioments. 

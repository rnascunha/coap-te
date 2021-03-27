# CoAP-te

**CoAP-te** is a C++17 implementation (work in progress) of CoAP protocol ([RFC7252](https://tools.ietf.org/html/rfc7252)).

The implemtation aims to:
* Don't use any *dynamic allocation*;
* Configurable: the CoAP *engine* be configurable to your system/environment requirements;
* Modularized: As much as possible, all modules work indenpenditly;
* Portable: simple to port to your system/device.

It's implemented:
* CoAP ([RFC7252](https://tools.ietf.org/html/rfc7252));
* Block-wise transfer ([RFC7959](https://tools.ietf.org/html/rfc7959));

It's NOT implemented (yet):
* Cache strategie;
* Security;
* Resource Discovery ([RFC6690](https://tools.ietf.org/html/rfc6690));
* Observing ([RFC7641](https://tools.ietf.org/html/rfc7641));
* TCP support ([RFC8323](https://tools.ietf.org/html/rfc8323));
* Much more :(...

## Dependencies

As external library dependency, **CoAP-te** uses just [Tree Trunks](https://github.com/rnascunha/tree_trunks) as log system (same author).

It also needs the following softwares:
* Compiler that support C++17 standard;
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

*Messages examples*:
* `serialize_parse`: shows how to serialize message using 3 strategies (factory and manually using option list/array). Then parse this information (as it was received by network) and iterate through options.
* `factory`: demostrate how to use a factory to construct a message, using internal/external buffers.
* `option`: shows how to manipulate options of different types. How to declare, serialize and parse.
* `serialze_class`: explain the use of the `Serialize` class. This is a class to modify the information that were
already serilized (written to the buffer).

*Transmission examples*:
* `raw_transaction`: explains the use of transactions. Transactions are not meant to be used directly, but through the *CoAP::engine*.
* `raw_engine`: demonstrate how to configure and use your own engines, the central feature of **CoAP-te**. Engines
deal with all CoAP transmission complexity. After configuration, makes a simple CoAP request.
* `engine_server`: shows how to add resource to a engine, and how to make the response to specific methods, using different strategies.
* `request_get_block_wise`: this example makes a *GET* request using *block2* block wise transfer, from a client to a server. Use with `response_block_wise` example. 
* `request_put_block_wise`: this example makes a *PUT* request using *block1* block wise transfer, from a client to a server. Use with `response_block_wise` example.
* `response_block_wise`: this example is a server that responds to the `request_get_block_wise` and `request_put_block_wise` examples above;

*URI examples*:
* `decompose`: breaks a CoAP URI into internal structures of **CoAP-te**, ready to be used. Any percent-encoded characters is converted. 

*Internal examples*: this examples are to test internal functions used, not intended to the end user.
* `tree`: shows how to work with *trees*, the structure that holds all the resources.
* `percent_encode`: demonstrate the use of function `percent_encode` used to compose URI strings;
* `percent_decode`: demonstrate the use of function `percent_decode` used to decompose URI strings;

*Port examples*: here are show the raw use of connections/plataform dependent functions. Not intended to the end user.
* `udp_server`: the use of the implemented posix-like UDP socket. Run a echo UDP server;
* `udp_client`: the use of the implemented posix-like UDP socket. Run a UDP client making a request and wait for a response;

## Portability

To port **CoAP-te** to your system, you must:
* Define the following functions:

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

* Define a connection. A connection must define a endpoint, a receiving function, and a send function:
  * The endpoint must be *default constructable*, *copiable* and *comparable*;
  * The class connection must be *moveable*. The receiving/send functions *MUST NOT block*. Any error calling the functions must be reported at the last parameter (*CoAP::errc::socket_error*).

```c++
struct my_connection{
	using endpoint = <some_endpoint>;

	std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
	std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
}
```
> There is already a implementation to UDP posix-like sockets.


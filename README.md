# CoAP-te

**CoAP-te** is a C++17 implementation (work in progress) of CoAP protocol ([RFC7252](https://tools.ietf.org/html/rfc7252)).

The implemtation aims to:
* Don't use any *dynamic allocation*;
* Configurable: the CoAP *engine* be configurable to your system/environment requirements;
* Modularized: As much as possible, all modules work indenpenditly;
* Portable: ease to port to your system/device.

It's NOT implemented (yet):
* Cache strategie;
* Security;
* Observing (RFC7641);
* Block-wise transfer (RFC7641);
* TCP support (RFC8323);
* Many more :(...

## Dependencies

As external library depencie, **CoAP-te** uses just [Tree Trunks](https://github.com/rnascunha/tree_trunks) as log system (same author).

It also needs the following softwares:
* Compiler that support C++17 standard;
* [git](https://git-scm.com/) to download;
* [CMake](https://cmake.org/) to build;

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
This will generate a library file (`libcoap-te.a` at linux). Link this file to your project, and include `./src/` directory. There are to convenient headers:
* `coap-te.hpp`: this has all necessary headers to use **CoAP-te**;
* `coap-te-debug.hpp`: this includes some print functions to help debug messages.

If will want also want to compile the examples (at `examples` directory), run at the build directory:

```
$ cmake -DWITH_EXAMPLES=1 ..
$ make
```

This will generate one excutable file to each example file.

## Using

The directory `examples` contains some code samples with detailed exaplanations on how to use **CoAP-te**. A brief overview is show here:

*Messages examples*:
* `serialize_parse`: shows how the serialize message using 3 strategies (factory and manually using option list/array). Then parse this information (as it was received by network) and iterate through options.
* `factory`: demostrate how to use a factory to construct a message, using internal/external features.
* `option`: shows how to manipulate options of different types. How to declare, serialize and parse.

*Transmission examples*:

*URI examples*:
* `decompose`: breaks a CoAP URI into internal structures of **CoAP-te**, ready to be used.

*Internal examples*: this examples are to test internal functions used, not intended to the end user.

*Port examples*: here are show the raw use of connections/plataform dependent functions. Not intended to the end user.

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

* Define a connection. A connection must define a endpoint, a receiving function, and a send function, as show:

```c++
struct my_connection{
	using endpoint = Endpoint;

	std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
	std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
}
```
  * The endpoint must be default constructable, copiable and comparable;
  * The class connection must be moveable. The receiving/send functions MUST NOT block.
> There is already a implmentation to UDP posix-like sockets.


# TODO

* Simplify;
* Type traits: all template parameters must be checked
* Change std::size_t to unsigned?
* Error report: error_code. Exceptions?
* Use some linter to enforce code style (clang-format?)
* reestructure code directories.
* Remove JSON functionality (make as a outside library)
* Do not hardcode log library (remove tree_trunks dependency)
* Benchmark library

## Compatibiliy

* C++17;
* Compilers: GCC, Clang, MSVC, MinGW;
* Work with:
* * ASIO
* * POSIX interface
* * ESP-MESH (no standard)

## Modules

### Message

* Make options iterable
* Better solution to function get_config from options.hpp
* Make Serialize class to reliable connection

### Resource

* https://tools.ietf.org/html/rfc6690 - Missing:
* * Need to make resource querying.
* * Construct the link format progressevelly (from packet to packet). I.e., if you don't have a buffer to fit all it won't work.
* Remove all function pointers that are not necessary at resource;

### URL

* Use a third-party library? (Boost.URL??)

### Engine

* Transaction restransmit paremeter as template...
* Transaction first parameter of callback is void. Should be the transaction. How?
* Same problem above... passing engine as a void*. Could be used type erause (std::functional<>), but this is a good option to embended devices?
* Max_Transmit_Span is really necessary?
* engine parameter profile and resource_callback seems to be ambiguos... is it?
* Add a statistic policy to collect network data.

### Cache

* Define a cache strategy.

## Document

* Use Doxygen to document

## Tests

* Create tests to each module of the library (Google Test?)
* ETSI tests.

## Others

### Languages bindings

Binding to languages like:
* WASM;
* Javascript;
* Python;
* ...

### Helper/Plugin library

* Message to JSON and JSON to message library;

### Tools

Web tool to construct and exchange menssage.


# Review

At the end, remember:

* Check byte order functions. Dealing only from little-endian to big-endian.
* Check lines with "//NOLINT"
* Run static analisys
* Check if all functions/class are documented
* Check if all functions/class are tested
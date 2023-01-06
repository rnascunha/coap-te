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
* Check lines with "//NOLINT":
* * header files like "system_error";
* * non-const references;
* Run static analisys
* Check if all functions/class are documented
* Check if all functions/class are tested
* Benchmark library:
* * binary size;
* * cpu time;

# Know issues

* At debug mode at MSVC, the checks in iterators at the binary_search asserts a error. This is a false error, as de bound is checked after.
* message/option uses create static function to create a option. This was necessary to make a compile options like checks/thorw avaiable (contructors can't explicitly set template options). Is options check really necessary?
* Some libraries are been added that may not be used/necessary/avaialble for embemded development, like containers/strings/ostream. Functions mostly used for development/debbugging.
* * Check the use of others library containers like ETL and EASTL (no heap container options);
* Test/use expected class (core/expected.hpp) or remove it;
* 'options list' types use 'count_options' to return the number of options and 'size' to return the serilized size of the message. Is this confusining?
* Also, equality (==) and less operator (<) just compare the option number, not the data. Is this also confusing?
* The iterator interface at const_buffer and mutable_buffer necessary?
* buffer_iterartor doesn`t accept arrays (T[]). Try to make it work...
* 'option_list_size' does not forbids the use of 'unordered_set/unordered_map' or 'map/set', unsuttable for the use as option container:
* * Document it;
* * Should it explicity forbid (make a type trait)?

# Improvements from ASIO Buffers (really??)

* is_*_buffer_sequence accepts arrays;
* buffers_iterator accepts array;
* Buffer iterator has a specialization to mutable_buffer and const_buffer. This makes reduce the class size and the processing needed.
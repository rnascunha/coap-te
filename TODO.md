# TODO

* Simplify;

* Transaction restransmit paremeter as template...

* Transaction first parameter of callback is void. Should be the transaction. How?

* Same problem above... passing engine as a void*. Could be used type erause (std::functional<>), but this is a good option to embended devices?

* Max_Transmit_Span is really necessary?

* Type traits...

* Define a cache strategy.

* Remove all function pointers that are not necessary at resource;

* Change std::size_t to unsigned?

* engine parameter profile and resource_callback seems to be ambiguos... is it?

* ETSI tests. Do it...

* https://tools.ietf.org/html/rfc6690 - Missing:
** Need to make resource querying.
** Construct the link format progressevelly (from packet to packet). I.e., if you don't have a buffer to fit all
it won't work.

* Better solution to function get_config from options.hpp

* Make Serialize class to reliable connection
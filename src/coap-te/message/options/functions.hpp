#ifndef COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP__
#define COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP__

#include "../types.hpp"
#include "types.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode>
bool is_critical(OptionCode code) noexcept;
template<typename OptionCode>
bool is_unsafe(OptionCode code) noexcept;
template<typename OptionCode>
bool is_no_cache_key(OptionCode code) noexcept;

#if	COAP_TE_BLOCKWISE_TRANSFER == 1

unsigned block_size(unsigned) noexcept;
unsigned block_szx(unsigned value) noexcept;
bool more(unsigned) noexcept;
unsigned block_number(unsigned) noexcept;
unsigned byte_offset(unsigned) noexcept;

bool make_block_raw(unsigned& value, unsigned number, bool more, unsigned size) noexcept;
bool make_block(unsigned& value, unsigned number, bool more, unsigned size) noexcept;

#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */

}//Option
}//Message
}//CoAP

#include "impl/functions_impl.hpp"

#endif /* COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP__ */

#include "options.hpp"
#include "parser.hpp"
#include "functions.hpp"

namespace CoAP{
namespace Message{
namespace Option{

void create(option& option,
		content_format const& value,
		bool is_request /* = false */) noexcept
{
	option.ocode = is_request ? code::accept : code::content_format;
	option.length = 1;
	option.value = &value;
}

#if	COAP_TE_BLOCKWISE_TRANSFER == 1

unsigned block_size(unsigned value) noexcept
{
	return CoAP::Helper::pow(2, (value & 0x7) + 4);
}

unsigned block_szx(unsigned value) noexcept
{
	return value & 0x7;
}

bool more(unsigned value) noexcept
{
	return value & 8;
}

unsigned block_number(unsigned value) noexcept
{
	return value >> 4;
}

unsigned byte_offset(unsigned value) noexcept
{
	return (value & ~0xF) << (value & 7);
}

bool make_block_raw(unsigned& value, unsigned number, bool more, unsigned size) noexcept
{
#if COAP_TE_RELIABLE_CONNECTION == 1
	if(!(/* size >= 0 && */ size <= 7)) return false;
#else
	if(!(/* size >= 0 && */ size <= 6)) return false;
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

	value =  number << 4 | more << 3 | size;

	return true;
}

bool make_block(unsigned& value, unsigned number, bool more, unsigned size) noexcept
{
	unsigned n_size = 0;
	while(true)
	{
		if(size % 2) return false;

		size = size >> 1;
		n_size++;
		if(size == 1) break;
	}

	return make_block_raw(value, number, more, n_size - 4);
}

#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */

}//Option
}//Message
}//CoAP


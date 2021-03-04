#ifndef COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__

#include "../engine.hpp"

namespace CoAP{
namespace Transmission{

template<typename Connection,
	configure const& Config,
	typename MessageID,
	typename Callback_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Connection, Config, MessageID, Callback_Functor>::
send(Request<Callback_Functor> const& req, CoAP::Error& ec) noexcept
{
	if constexpr(UseInternalBufferNon)
	{
				if(req.type() == CoAP::Message::type::nonconfirmable)
					return req.serialize<SortOptions,
							CheckOpOrder,
							CheckOpRepeat>(buffer_, Config.max_packet_size, mid_(), ec);
	}
	transaction_t* ts = list_.check_free_slot();
	if(!ts)
	{
		ec = CoAP::errc::no_free_slots;
		return 0;
	}
	std::size_t size = ts->template request<SortOptions, CheckOpOrder, CheckOpRepeat>(req, 10, ec);

	return size;
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */

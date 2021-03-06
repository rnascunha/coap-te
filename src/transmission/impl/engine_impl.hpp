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
	std::size_t size = 0;
	if constexpr(UseInternalBufferNon)
	{
		if(req.type() == CoAP::Message::type::nonconfirmable)
		{
			size = req.serialize<SortOptions,
					CheckOpOrder,
					CheckOpRepeat>(buffer_, Config.max_packet_size, mid_(), ec);
			if(ec) return 0;
			if(size)
			{
				conn_.send(buffer_, size, req.endpoint(), ec);
				if(ec)
					CoAP::Log::error(engine_mod, ec, "send");

				return size;
			}
		}
	}
	else
	{
		transaction_t* ts = list_.check_free_slot();
		if(!ts)
		{
			ec = CoAP::errc::no_free_slots;
			return 0;
		}
		size = ts->template request<SortOptions, CheckOpOrder, CheckOpRepeat>(req, 10, ec);
		conn_.send(ts->buffer(), size, ts->endpoint(), ec);
	}
	return size;
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */

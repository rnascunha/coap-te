#ifndef COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__

#include "../engine.hpp"

namespace CoAP{
namespace Transmission{

template<typename Connection,
	configure const& Config,
	typename MessageID,
	typename Callback_Functor>
template<bool iUseInternalBufferNon,
				bool iSortOptions2,
				bool iCheckOpOrder2,
				bool iCheckOpRepeat2>
std::size_t
engine<Connection, Config, MessageID, Callback_Functor>::
send2(Request2<Callback_Functor> const& req2, CoAP::Error& ec) noexcept
{
	if constexpr(iUseInternalBufferNon)
	{
//				if(req2.type() == CoAP::Message::type::nonconfirmable)
//					return req2.serialize<SortOptions2,
//							false/*CheckOpOrder*/,
//							CheckOpRepeat2>(buffer_, Config.max_packet_size, mid_(), ec);
	}
	transaction_t* ts2 = list_.check_free_slot();
	if(!ts2)
	{
		ec = CoAP::errc::no_free_slots;
		return 0;
	}
//			std::size_t size2 = ts2->request2(req2, 10, ec);
	std::size_t size2 = ts2->request2<iSortOptions2, iCheckOpOrder2, iCheckOpRepeat2>(req2, 10, ec);
//			std::size_t size2 = ts2->request2<SortOptions2, false/*CheckOpOrder*/, CheckOpRepeat2>(
//					req2, mid_(), ec);

	return size2;
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */

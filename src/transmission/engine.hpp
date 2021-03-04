#ifndef COAP_TE_TRANSMISSION_ENGINE_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_HPP__

#include <cstdint>

#include "error.hpp"

#include "types.hpp"
#include "transaction.hpp"
#include "transaction_list.hpp"
#include "request.hpp"

#include "message/message_id.hpp"

namespace CoAP{
namespace Transmission{

template<typename Connection,
	configure const& Config,
	typename MessageID,
	typename Callback_Functor = transaction_cb>
class engine
{
	public:
		using transaction_t = transaction2<Config.max_packet_size, Callback_Functor>;
		engine(Connection&& conn, MessageID&& mid)
		: conn_(std::move(conn)), mid_(std::move(mid)){}

		template<bool iUseInternalBufferNon,
				bool iSortOptions2,
				bool iCheckOpOrder2,
				bool iCheckOpRepeat2>
		std::size_t send2(Request2<Callback_Functor> const& req2, CoAP::Error& ec) noexcept;

		void process(std::uint8_t const* buffer, std::size_t buffer_len) noexcept
		{
			Response response;
			CoAP::Error ec;
			parse(response, buffer, buffer_len, ec);
			if(ec) return;

			transaction_t* trans = list_.mid(response.mid);
			if(!trans) return;

			trans->call_cb(&response);
			list_.clear_by_mid(response.mid);
		}

		void check_transactions()
		{
			list_.checks(CoAP::time());
		}

		bool operator()(CoAP::Error& ec)
		{
			CoAP::endpoint ep;
			std::size_t size = conn_.receive(buffer_, Config.max_packet_size, ep, ec);
			if(ec) return false;
			process(buffer_, size);

			check_transactions();

			return true;
		}
	private:
		transaction_list<Config.max_interaction,
						 Config.max_packet_size,
						 Callback_Functor> list_;
		Connection		conn_;
		MessageID		mid_;
		std::uint8_t	buffer_[Config.max_packet_size];
};

}//Transmission
}//CoAP

#include "impl/engine_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_ENGINE_HPP__ */

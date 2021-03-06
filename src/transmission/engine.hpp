#ifndef COAP_TE_TRANSMISSION_ENGINE_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_HPP__

#include <cstdint>

#include "error.hpp"

#include "types.hpp"
#include "transaction.hpp"
#include "transaction_list.hpp"
#include "request.hpp"

#include "message/message_id.hpp"

#include "debug/print_message.hpp"
#include "log.hpp"

namespace CoAP{
namespace Transmission{

static constexpr CoAP::Log::module engine_mod = {
		.name = "ENGINE",
		.max_level = CoAP::Log::type::debug,
		.enable = true
};

template<typename Connection,
	configure const& Config,
	typename MessageID,
	typename Callback_Functor = transaction_cb>
class engine
{
	public:
		using transaction_t = transaction<Config.max_packet_size, Callback_Functor>;
		engine(Connection&& conn, MessageID&& mid)
		: conn_(std::move(conn)), mid_(std::move(mid)){}

		template<bool UseInternalBufferNon,
				bool SortOptions,
				bool CheckOpOrder,
				bool CheckOpRepeat>
		std::size_t send(Request<Callback_Functor> const&, CoAP::Error&) noexcept;

		void process(std::uint8_t const* buffer, std::size_t buffer_len) noexcept
		{
			Response response;
			CoAP::Error ec;
			parse(response, buffer, buffer_len, ec);
			if(ec)
			{
				error(engine_mod, ec, "parsing response");
				return;
			}
			CoAP::Debug::print_message(response);

			transaction_t* trans = list_.mid(response.mid);
			if(!trans)
			{
				warning(engine_mod, "Transaction %u not found", response.mid);
				return;
			}

			debug(engine_mod, "Calling callback mid[%u]", response.mid);
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
			if(size)
			{
				CoAP::Log::debug(engine_mod, "Received %d bytes", size);
				process(buffer_, size);
			}

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

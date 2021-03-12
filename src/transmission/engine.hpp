#ifndef COAP_TE_TRANSMISSION_ENGINE_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_HPP__

#include <cstdint>

#include "error.hpp"

#include "types.hpp"
#include "transaction_list.hpp"
#include "transaction.hpp"
#include "request.hpp"
#include "response.hpp"
#include "resource/types.hpp"
#include "resource/node.hpp"
//#include "resource/list.hpp"

namespace CoAP{
namespace Transmission{

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor = transaction_cb,
	typename Callback_Resource_Functor = CoAP::Resource::callback>
class engine
{
	public:
		using endpoint = typename Connection::endpoint;
		using transaction_t = transaction<MaxPacketSize, Callback_Functor, endpoint>;
		using request = Request<Callback_Functor>;
		using resource = CoAP::Resource::resource<Callback_Resource_Functor>;
		using resource_root = typename CoAP::Resource::resource_node<Callback_Resource_Functor>;
		using resource_node = typename resource_root::node_t;

		engine(Connection&& conn)
		: conn_(std::move(conn)){}

		template<bool UseInternalBufferNon,
				std::size_t BufferSize,
				typename MessageID,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, MessageID>&,
				Callback_Functor func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon,
				std::size_t BufferSize,
				typename MessageID,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, MessageID> const&,
				std::uint16_t mid,
				Callback_Functor func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(Request<Callback_Functor>&,
						std::uint16_t mid,
						CoAP::Error&) noexcept;

		void add_resource(resource_node& res) noexcept
		{
			resource_root_.template add_child<true>(res);
		}

		resource& root() noexcept{ return resource_root_.root(); }
		resource_node& root_node() noexcept{ return resource_root_.node(); }

		template<bool UseEndpointTransMatch = false>
		void process(endpoint& ep,
				std::uint8_t const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;
		void check_transactions() noexcept;
		bool run(CoAP::Error& ec);
		bool operator()(CoAP::Error& ec) noexcept;
	private:
		transaction_list<MaxTransactionNumber,
						transaction_t> list_;

		resource_root	resource_root_;

		Connection		conn_;
		std::uint8_t	buffer_[MaxPacketSize];
};

}//Transmission
}//CoAP

#include "impl/engine_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_ENGINE_HPP__ */

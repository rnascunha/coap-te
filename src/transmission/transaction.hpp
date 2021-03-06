#ifndef COAP_TE_TRANSMISSION_TRANSACTION_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_HPP__

#include <cstdint>
#include <cstdlib>
#include <type_traits>

//#include "port/port.hpp"
#include "transmission/types.hpp"
#include "request.hpp"
#include "message/types.hpp"
#include "message/parser.hpp"

#include "log.hpp"

namespace CoAP{
namespace Transmission{

static constexpr CoAP::Log::module transaction_mod = {
		.name = "TRANS",
		.max_level = CoAP::Log::type::debug,
		.enable = true
};

template<unsigned MaxPacketSize = 0,
		typename Callback_Functor = transaction_cb,
		typename Endpoint = CoAP::endpoint>
class transaction
{
	static bool constexpr const is_external_storage = MaxPacketSize == 0;
	using buffer_type = typename std::conditional<MaxPacketSize == 0, std::uint8_t const*, std::uint8_t[MaxPacketSize]>::type;
	public:
		transaction(){}

		enum class status_t{
			none = 0,
			sending,
			canceled,
			success,
			timeout
		};

		status_t status() const noexcept{ return status_; }

		void clear() noexcept;

		bool check(configure const& config) noexcept;
		bool check(CoAP::Message::message const& response);
		bool check(Endpoint const& ep,
				CoAP::Message::message const& response);

		std::uint16_t mid() const noexcept;
		std::uint8_t* buffer() noexcept;
		Endpoint const& endpoint() const noexcept;

		bool init(Endpoint const& ep, std::uint8_t const* buffer, std::size_t size,
						Callback_Functor func, void* data, CoAP::Error& ec);
//		template<bool SortOptions,
//				bool CheckOpOrder,
//				bool CheckOpRepeat>
//		std::size_t request(Request<Callback_Functor> const& req,
//				uint16_t mid,
//				CoAP::Error& ec) noexcept
//		{
//			std::size_t size = req.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
//														buffer_, MaxPacketSize, mid, ec);
//			if(ec)
//			{
//				error(transaction_mod, ec, "serialize");
//				return size;
//			}
//
//			if(req.factory().type() != CoAP::Message::type::confirmable) return size;
//			transmission_init_ = CoAP::time();
//			retransmission_count_ = 0;
//			buffer_used_ = size;
//			/**
//			 * Parsing and serialize hold the message in different ways...
//			 */
//			CoAP::Message::parse(request_, buffer_, MaxPacketSize, ec);
//
//			ep_ = req.endpoint();
//			cb_ = req.callback();
//			data_ = req.data();
//
//			return size;
//		}
	private:
		void call_cb(CoAP::Message::message const* response) noexcept;

		CoAP::Message::message	request_;
		Endpoint			ep_;

		Callback_Functor		cb_ = nullptr;
		void*					data_ = nullptr;
		CoAP::time_t			transmission_init_ = 0;
		CoAP::time_t			expiration_time_ = 0;
		unsigned int			retransmission_count_ = 0;
		buffer_type				buffer_;
		std::size_t				buffer_used_ = 0;

		status_t				status_ = status_t::none;
};

}//Transmission
}//CoAP

#include "impl/transaction_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_HPP__ */

#ifndef COAP_TE_TRANSMISSION_TRANSACTION_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_HPP__

#include <cstdint>
#include <cstdlib>

//#include "port/port.hpp"
#include "request.hpp"
#include "message/types.hpp"
#include "message/parser.hpp"

namespace CoAP{
namespace Transmission{

template<unsigned MaxPacketSize,
		typename Callback_Functor>
class transaction
{
	public:
		transaction(){}

		void call_cb(CoAP::Message::message const* response) const noexcept
		{
			if(cb_) cb_(&request_, response, data_);
		}

		bool check(CoAP::time_t time)
		{
//			if(time > expiration_time_)
//			{
//				if(retransmission_count_ >= )
//			}
			return true;
		}

		void clear()
		{
			cb_ = nullptr;
			data_ = nullptr;
			transmission_init_ = 0;
			expiration_time_ = 0;
			retransmission_count_ = 0;
			buffer_used_ = 0;
		}

		std::uint16_t mid() const noexcept{ return request_.mid; }

		std::uint8_t* buffer() noexcept { return buffer_; }

		template<bool SortOptions,
				bool CheckOpOrder,
				bool CheckOpRepeat>
		std::size_t request(Request<Callback_Functor> const& req,
				uint16_t mid,
				CoAP::Error& ec) noexcept
		{
			std::size_t size = req.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
														buffer_, MaxPacketSize, mid, ec);

			if(req.factory().type() != CoAP::Message::type::confirmable) return size;
			transmission_init_ = CoAP::time();
			retransmission_count_ = 0;
			buffer_used_ = size;
			/**
			 * Parsing and serialize hold the message in different ways...
			 */
			CoAP::Message::parse(request_, buffer_, MaxPacketSize, ec);

			ep_ = req.endpoint();
			cb_ = req.callback();
			data_ = req.data();

			req.reset();

			return size;
		}
	private:
		CoAP::Message::message	request_;
		CoAP::endpoint			ep_;

		Callback_Functor		cb_ = nullptr;
		void*					data_ = nullptr;
		CoAP::time_t			transmission_init_ = 0;
		CoAP::time_t			expiration_time_ = 0;
		unsigned int			retransmission_count_ = 0;
		std::uint8_t			buffer_[MaxPacketSize];
		std::size_t				buffer_used_ = 0;
};

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_HPP__ */

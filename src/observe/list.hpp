#ifndef COAP_TE_OBSERVE_LIST_HPP__
#define COAP_TE_OBSERVE_LIST_HPP__

#include "types.hpp"

namespace CoAP{
namespace Observe{

template<typename Observe,
		unsigned Size>
class list{
	public:
		using endpoint = typename Observe::endpoint_t;

		list();

		constexpr unsigned size() const noexcept;

		template<typename Message, typename ...Args>
		bool process(endpoint const&, Message const&, Args&&...) noexcept;

		template<typename Message, typename ...Args>
		bool add(endpoint const&, Message const&, Args&&...) noexcept;
		template<typename Message>
		bool remove(endpoint const&, Message const&) noexcept;

		void cancel() noexcept;
		void cancel(endpoint const&) noexcept;

		Observe* operator[](unsigned index) noexcept;
	private:
		Observe	list_[Size];
};

}//Observe
}//CoAP

#include "impl/list_impl.hpp"

#endif /* COAP_TE_OBSERVE_LIST_HPP__ */

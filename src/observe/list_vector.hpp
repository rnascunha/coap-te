#ifndef COAP_TE_OBSERVE_LIST_VECTOR_HPP__
#define COAP_TE_OBSERVE_LIST_VECTOR_HPP__

#include "types.hpp"
#include <vector>

namespace CoAP{
namespace Observe{

template<typename Observe>
class list_vector{
	public:
		using endpoint = typename Observe::endpoint_t;

		list_vector();

		unsigned size() const noexcept;

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
		std::vector<Observe> list_;
};

}//Observe
}//CoAP

#include "impl/list_vector_impl.hpp"

#endif /* COAP_TE_OBSERVE_LIST_VECTOR_HPP__ */

#ifndef COAP_TE_RESOURCE_NODE_HPP__
#define COAP_TE_RESOURCE_NODE_HPP__

#include "internal/tree.hpp"
#include "resource.hpp"
#include "message/parser.hpp"
#include "message/options.hpp"

namespace CoAP{
namespace Resource{

template<typename Callback_Functor>
class resource_node{
	public:
		using resource_t = resource<Callback_Functor>;
		using node_t = typename CoAP::branch<resource_t>;

		resource_node() : root_(nullptr){}

		resource_t& root() noexcept { return root_.value(); }
		node_t& node() noexcept { return root_; }

		template<typename ...Args>
		void add_branch(Args&&... args) noexcept
		{ root_.template add_branch<false>(std::forward<Args>(args)...); }

		template<typename ...Args>
		bool add_child(Args&&... args) noexcept
		{ return root_.template add_child<true, false>(std::forward<Args>(args)...); }

		resource_t const* search(CoAP::Message::message& msg) noexcept
		{
			using namespace CoAP::Message;
			using namespace CoAP::Message::Option;
			resource_t* res = &root_.value();

			Option_Parser op(msg);
			option const* opt;
			node_t* n = &root_;
			while((opt = op.next()))
			{
				if(opt->ocode == Option::code::uri_path)
				{
					n = n->find_child(*opt);
					if(!n) return nullptr;
					res = &n->value();
				}
			}

			return res;
		}

		node_t* operator[](unsigned index) noexcept { return root_[index]; }
	private:
		node_t root_;
};

}//Resource
}//CoAP


#endif /* COAP_TE_RESOURCE_NODE_HPP__ */

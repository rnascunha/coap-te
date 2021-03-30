#ifndef COAP_TE_RESOURCE_NODE_HPP__
#define COAP_TE_RESOURCE_NODE_HPP__

#include "internal/tree.hpp"
#include "resource.hpp"
#include "message/parser.hpp"
#include "message/options.hpp"

namespace CoAP{
namespace Resource{

template<typename Resource>
class resource_root{
	public:
		using resource_t = Resource;
		using node_t = typename CoAP::branch<resource_t>;

		resource_root() : root_(nullptr){}

		resource_t& root() noexcept { return root_.value(); }
		node_t& node() noexcept { return root_; }

		template<typename ...Args>
		void add_branch(Args&&... args) noexcept
		{
			root_.template add_branch<false>(std::forward<Args>(args)...);
		}

		template<typename ...Args>
		bool add_child(Args&&... args) noexcept
		{
			return root_.template add_child<true, false>(std::forward<Args>(args)...);
		}

		node_t* search_node(CoAP::Message::message const& msg) noexcept
		{
			using namespace CoAP::Message;
			using namespace CoAP::Message::Option;
			node_t* res = &root_;

			Option_Parser op(msg);
			option const* opt;
			node_t* n = &root_;
			while((opt = op.next()))
			{
				if(opt->ocode == Option::code::uri_path)
				{
					n = n->find_child(*opt);
					if(!n) return nullptr;
					res = n;
				}
			}
			return res;
		}

		node_t* search_parent(CoAP::Message::message const& msg) noexcept
		{
			using namespace CoAP::Message;
			using namespace CoAP::Message::Option;
			node_t const* res = &root_;

			Option_Parser op(msg);
			option const* opt;
			node_t const* n = &root_, *parent = nullptr;
			while((opt = op.next()))
			{
				if(opt->ocode == Option::code::uri_path)
				{
					parent = n;
					n = n->find_child(*opt);
					if(!n) return nullptr;
					res = n;
				}
			}
			return parent;
		}

		resource_t const* search(CoAP::Message::message const& msg) noexcept
		{
			node_t const* node = search_node(msg);
			return node ? &node->value() : nullptr;
		}

		node_t* remove_node(CoAP::Message::message const& msg) noexcept
		{
			using namespace CoAP::Message;
			using namespace CoAP::Message::Option;

			Option_Parser op(msg);
			option const* opt;
			node_t* n = &root_, *parent = nullptr;
			while((opt = op.next()))
			{
				if(opt->ocode == Option::code::uri_path)
				{
					parent = n;
					n = n->find_child(*opt);
					if(!n) return nullptr;
				}
			}
			if(parent)
				parent->remove_child(*n);
			return n;
		}

		node_t* operator[](unsigned index) noexcept { return root_[index]; }
	private:
		node_t root_;
};

}//Resource
}//CoAP


#endif /* COAP_TE_RESOURCE_NODE_HPP__ */

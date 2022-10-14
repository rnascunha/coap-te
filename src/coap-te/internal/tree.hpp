#ifndef COAP_TE_INTERNAL_TREE_HPP__
#define COAP_TE_INTERNAL_TREE_HPP__

#include <utility>

namespace CoAP{

template<typename T>
class branch{
	public:
		using type = T;

		template<typename ...Args>
		branch(Args&&... args) : value_(std::forward<Args>(args)...){}

		T& value() noexcept{ return value_; }
		T const& value() const noexcept{ return value_; }
		branch* next() noexcept{ return next_; }
		branch const* next() const noexcept{ return next_; }
		branch const* children() const noexcept { return children_; }

		template<typename U>
		branch* find_child(U const& u) noexcept
		{
			branch* n = children_;
			for(; n; n = n->next_)
				if(n->value_ == u) return n;

			return nullptr;
		}

		template<typename U>
		branch* find_descendant(U const** u, unsigned depth) noexcept
		{
			branch* n = this;
			for(unsigned i = 0; i < depth; i++)
			{
				n = n->find_child(*u[i]);
				if(!n) return nullptr;
			}
			return n;
		}

/**
 * MSVC warning... don't understand...
 */ 
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4702 )
#endif /* defined(_MSC_VER) */

		template<typename Path, typename ...Paths>
		branch* find_descendant(Path path, Paths&& ... paths) noexcept
		{
			branch* n = find_child(path);
			if(!n) return nullptr;
			if constexpr(sizeof ...(paths) > 0)
			{
				return n->find_descendant(std::forward<Paths>(paths)...);
			}
			return n;
		}

		template<typename U>
		branch* operator[](U const& u)
		{
			return find_child(u);
		}

		branch* operator[](unsigned index) noexcept
		{
			branch* n = children_;
			for(;index-- && n; n = n->next());
			return n;
		}

		template<bool CheckRepeat = true, bool AddSorted = false, typename Path, typename ...Paths>
		bool add_child(Path& path, Paths&& ...paths) noexcept
		{
			bool valid = add_child(path);
			if(!valid) return valid;

			if constexpr(sizeof...(paths) > 0)
			{
				return  add_child<CheckRepeat, AddSorted>(std::forward<Paths>(paths)...);
			}
			return valid;
		}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif /* defined(_MSC_VER) */

		template<bool CheckRepeat = true, bool AddSorted = false>
		bool add_child(branch& new_node) noexcept
		{
			if constexpr(CheckRepeat)
			{
				if(find_child(new_node.value())) return false;
			}

			if(children_ == nullptr)
			{
				children_ = &new_node;
				children_->next_ = nullptr;
				return true;
			}

			if constexpr(AddSorted)
			{
				branch *no = children_, *prev = nullptr;
				for(; no != nullptr; no = no->next)
				{
					if(new_node.value() < no->value())
					{
						new_node.next = no;
						if(prev) prev->next = &new_node;
						else children_ = &new_node;
						break;
					}
					if(no->next == nullptr)
					{
						no->next = &new_node;
						new_node.next = nullptr;
						break;
					}
					prev = no;
				}
			}
			else
			{
				branch* no = children_;
				for(; no->next_ != nullptr; no = no->next_);
				no->next_ = &new_node;
				new_node.next_ = nullptr;
			}
			return true;
		}

		template<typename U, bool CheckRepeat = true>
		bool add_descendant(branch& n, U const** u, unsigned depth) noexcept
		{
			T* f = find_descendant(u, depth);
			if(!f) return false;

			return f->template add_child<CheckRepeat>(n);
		}

		template<bool AddSorted = false, typename Path, typename ...Paths>
		void add_branch(Path& path, Paths&& ...paths) noexcept
		{
			add_child<true, AddSorted>(path);
			if constexpr(sizeof...(paths) > 0)
			{
				path.template add_branch<AddSorted>(std::forward<Paths>(paths)...);
			}
		}

		template<typename U>
		branch* remove_child(U const& u) noexcept
		{
			if(!children_) return nullptr;

			branch* n = children_;
			if(children_->value_ == u)
			{
				children_ = children_->next_;
				n->next_ = nullptr;
				return n;
			}

			for(; n->next_; n = n->next_)
			{
				if(n->next_->value_ == u)
				{
					branch* res = n->next_;
					n->next_ = n->next_->next_;
					res->next_ = nullptr;
					return n;
				}
			}

			return nullptr;
		}

		branch* remove_child(branch& u) noexcept
		{
			if(!children_) return nullptr;

			if(children_ == &u)
			{
				children_ = u.next_;
				u.next_ = nullptr;
				return &u;
			}

			branch* n = children_;
			for(; n->next_; n = n->next_)
			{
				if(n->next_ == &u)
				{
					n->next_ = u.next_;
					u.next_ = nullptr;
					return &u;
				}
			}

			return nullptr;
		}
	private:
		T				value_;
		branch*			children_ = nullptr;
		branch*			next_ = nullptr;
};

}//CoAP

#endif /* COAP_TE_INTERNAL_TREE_HPP__ */

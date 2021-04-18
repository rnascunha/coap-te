#ifndef COAP_TE_RESOURCE_LINK_FORMAT_HPP__
#define COAP_TE_RESOURCE_LINK_FORMAT_HPP__

#include "types.hpp"
#include <cstdlib>

namespace CoAP{
namespace Resource{
namespace Link_Format{


class Parser{
	public:
		Parser(const char*, std::size_t);
		link_format const* next() noexcept;

		void reset() noexcept;
	private:
		link_format		link_;

		char const*		buffer_;
		std::size_t 	buffer_len_;
		std::size_t		offset_ = 0;
};

class Parser_Attr{
	public:
		Parser_Attr(link_format const*);
		Parser_Attr(const char*, std::size_t);
		attribute const* next() noexcept;

		void reset() noexcept;
	private:
		attribute	attr_;

		char const*			buffer_;
		std::size_t 		buffer_len_;
		std::size_t			offset_ = 0;
};

class Parser_Value{
	public:
		Parser_Value(attribute const*);
		Parser_Value(const char*, std::size_t);
		link_value const* next() noexcept;

		void reset() noexcept;
	private:
		link_value				value_;

		char const*			buffer_;
		std::size_t 		buffer_len_;
		std::size_t			offset_ = 0;
};

}//Link_Format
}//Resource
}//CoAP

#endif /* COAP_TE_RESOURCE_LINK_FORMAT_HPP__ */

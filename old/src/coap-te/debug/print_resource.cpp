#include "../resource/link_format.hpp"
#include "print_resource.hpp"

namespace CoAP{
namespace Debug{

void print_link_format(const char* buffer, std::size_t length) noexcept
{
	using namespace CoAP::Resource::Link_Format;
	Parser parser(buffer, length);
	link_format const* lf;
	while((lf = parser.next()) != nullptr)
	{
		std::printf("[%zu] %.*s\n",
				lf->link_len, static_cast<int>(lf->link_len), lf->link);
		Parser_Attr parser_attr(lf->description, lf->desc_len);
		attribute const* la;
		while((la = parser_attr.next()) != nullptr)
		{
			std::printf("\tattr[%zu]: %.*s\n",
					la->attr_len, static_cast<int>(la->attr_len), la->attr);
			Parser_Value parser_value(la);
			link_value const* lv;
			while((lv = parser_value.next()) != nullptr)
			{
				std::printf("\t\tvalue[%zu]: %.*s\n",
						lv->value_len, static_cast<int>(lv->value_len), lv->value);
			}
		}
	}
}

}//Debug
}//CoAP

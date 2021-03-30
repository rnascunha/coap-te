#include "resource/link_format.hpp"
#include "print_resource.hpp"

namespace CoAP{
namespace Debug{

void print_link_format(const char* buffer, std::size_t length) noexcept
{
	using namespace CoAP::Resource::Link_Format;
	Parser parser(buffer, length);
	link_format const* lf;
	while((lf = parser.next()))
	{
		std::printf("[%lu] %.*s\n",
				lf->link_len, static_cast<int>(lf->link_len), lf->link);
		Parser_Attr parser_attr(lf->description, lf->desc_len);
		attribute const* la;
		while((la = parser_attr.next()))
		{
			std::printf("\tattr[%lu]: %.*s\n",
					la->attr_len, static_cast<int>(la->attr_len), la->attr);
			Parser_Value parser_value(la);
			value const* lv;
			while((lv = parser_value.next()))
			{
				std::printf("\t\tvalue[%lu]: %.*s\n",
						lv->value_len, static_cast<int>(lv->value_len), lv->value);
			}
		}
	}
}

}//Debug
}//CoAP

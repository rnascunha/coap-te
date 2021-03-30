#include "link_format.hpp"
#include <cstdio>

namespace CoAP{
namespace Resource{
namespace Link_Format{

Parser::Parser(const char* buffer, std::size_t length)
	: buffer_(buffer), buffer_len_(length){}

link_format const*
Parser::next() noexcept
{
	if(offset_ >= buffer_len_) return nullptr;
	link_.reset();

	std::size_t i = offset_;
	for(; i < buffer_len_; i++)
	{
		if(buffer_[i] == ',') break;
		if(buffer_[i] == ';')
		{
			if(!link_.description)
			{
				link_.description = &buffer_[i + 1];
				continue;
			}
		}
		if(link_.description) link_.desc_len++;
		else link_.link_len++;
	}

	auto off = offset_;

	offset_ = i + 1;
	if(buffer_[off] != '<' || buffer_[off + link_.link_len - 1] != '>')
		return nullptr;

	link_.link = &buffer_[off + 1];
	link_.link_len -= 2;

	return &link_;
}

void Parser::reset() noexcept
{
	offset_ = 0;
}

Parser_Attr::Parser_Attr(const char* buffer, std::size_t length)
: buffer_(buffer), buffer_len_(length){}

Parser_Attr::Parser_Attr(link_format const* lf)
	: Parser_Attr(lf->description, lf->desc_len){}

attribute const* Parser_Attr::next() noexcept
{
	if(offset_ >= buffer_len_) return nullptr;
	attr_.reset();

	attr_.attr = &buffer_[offset_];

	std::size_t i = offset_;
	for(; i < buffer_len_; i++)
	{
		if(buffer_[i] == ';') break;
		if(buffer_[i] == '=')
		{
			if(!attr_.value)
			{
				attr_.value = &buffer_[i + 1];
			}
		}
		if(attr_.value) attr_.value_len++;
		else attr_.attr_len++;
	}

	if(attr_.value_len && (buffer_[i] == ',' || buffer_[i] == ';'))
		attr_.value_len--;

	offset_ = i + 1;

	return &attr_;
}

void Parser_Attr::reset() noexcept
{
	offset_ = 0;
}

Parser_Value::Parser_Value(attribute const* attr)
	: Parser_Value(attr->value, attr->value_len){}

Parser_Value::Parser_Value(const char* buffer, std::size_t length)
	: buffer_(buffer), buffer_len_(length){}

value const* Parser_Value::next() noexcept
{
	if(offset_ >= buffer_len_) return nullptr;
	value_.reset();

	value_.value = &buffer_[offset_];

	std::size_t i = offset_;
	bool flag_quote = false;
	for(; i < buffer_len_; i++)
	{
		if(buffer_[i] == ';') break;
		if(buffer_[i] == ' ')
			if(!flag_quote) break;
		if(buffer_[i] == '"')
			flag_quote = !flag_quote;
		value_.value_len++;
	}

	offset_ = i + 1;

	return &value_;
}

void Parser_Value::reset() noexcept
{
	offset_ = 0;
}

}//Link_Format
}//Resource
}//CoAP

#include "decompose.hpp"
#include "internal/ascii.hpp"
#include "internal/decoder.hpp"
#include <cstring>

namespace CoAP{
namespace URI{

static bool split_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* data, std::size_t data_length,
				char delimiter, CoAP::Message::Option::code code,
				CoAP::Message::Option::List& list) noexcept;
static bool split_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				char* data, std::size_t data_length,
				char delimiter, CoAP::Message::Option::code code,
				CoAP::Message::Option::List& list) noexcept;

bool path_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				char* path, std::size_t path_length,
				CoAP::Message::Option::List& list) noexcept
{
	return split_to_list(buffer, buffer_len,
			path, path_length,
			'/', CoAP::Message::Option::code::uri_path,
			list);
}

bool query_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				char* query, std::size_t query_length,
				CoAP::Message::Option::List& list) noexcept
{
	return split_to_list(buffer, buffer_len,
			query, query_length,
			'&', CoAP::Message::Option::code::uri_query,
			list);
}

bool path_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* path, std::size_t path_length,
				CoAP::Message::Option::List& list) noexcept
{
	return split_to_list(buffer, buffer_len,
			path, path_length,
			'/', CoAP::Message::Option::code::uri_path,
			list);
}

bool query_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* query, std::size_t query_length,
				CoAP::Message::Option::List& list) noexcept
{
	return split_to_list(buffer, buffer_len,
			query, query_length,
			'&', CoAP::Message::Option::code::uri_query,
			list);
}

/**
 *
 */

static bool split_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				char* data, std::size_t data_length,
				char delimiter, CoAP::Message::Option::code ocode,
				CoAP::Message::Option::List& list) noexcept
{
	using namespace CoAP::Message;

	if(!data_length)
	{
		buffer_len = 0;
		return true;
	}

	std::size_t j = 0, total = buffer_len;
	for(std::size_t i = 0; i <= data_length; i++)
	{
		if(i == data_length || data[i] == delimiter)
		{
			if(buffer_len < sizeof(Option::node)) return false;
			Option::node* node = reinterpret_cast<Option::node*>(buffer);
			buffer_len -= sizeof(Option::node);
			std::size_t length;// = buffer_len;

			buffer += sizeof(Option::node);
			if(!(length = CoAP::Helper::percent_decode(data + j, i - j)))
			{
				return false;
			}
			node->value.ocode = ocode;
			node->value.length = static_cast<unsigned>(length);
			node->value.value = data + j;

			list.add<true>(*node);

			j = i + 1;
		}
	}

	buffer_len = total - buffer_len;

	return true;
}

static bool split_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* data, std::size_t data_length,
				char delimiter, CoAP::Message::Option::code ocode,
				CoAP::Message::Option::List& list) noexcept
{
	using namespace CoAP::Message;

	if(!data_length)
	{
		buffer_len = 0;
		return true;
	}

	std::size_t j = 0, total = buffer_len;
	for(std::size_t i = 0; i <= data_length; i++)
	{
		if(i == data_length || data[i] == delimiter)
		{
			if(buffer_len < sizeof(Option::node)) return false;
			Option::node* node = reinterpret_cast<Option::node*>(buffer);
			buffer_len -= sizeof(Option::node);
			std::size_t length = buffer_len;

			buffer += sizeof(Option::node);
			if(!(length = CoAP::Helper::percent_decode(reinterpret_cast<char*>(buffer), buffer_len, data + j, i - j)))
			{
				return false;
			}
			node->value.ocode = ocode;
			node->value.length = static_cast<unsigned>(length);
			node->value.value = buffer;

			buffer_len -= length;
			buffer += length;

			list.add<true>(*node);

			j = i + 1;
		}
	}

	buffer_len = total - buffer_len;

	return true;
}

}//URI
}//CoAP

#ifndef COAP_TE_HELPER_ENCODE_IMPL_HPP__
#define COAP_TE_HELPER_ENCODE_IMPL_HPP__

#include "../encoder.hpp"
#include <cstdio>
#include <cstring>

namespace CoAP{
namespace Helper{

struct encoder_list{
	encoder_list(const char* list, std::size_t size) : list_(list), size_(size){}

	bool operator()(char c)
	{
		for(std::size_t i = 0; i < size_; i++)
		{
			if(list_[i] == c) return true;
		}
		return false;
	}

	const char* list_;
	std::size_t size_;
};

template<typename Functor>
std::size_t percent_encoded_size(const char* buffer, std::size_t buffer_used,
								Functor func,
								std::size_t* changes /* = nullptr */)
{
	std::size_t size_after_encoded = 0;
	if(changes) *changes = 0;

	for(std::size_t i = 0; i < buffer_used; i++)
	{
		if(func(buffer[i]))
		{
			if(changes) *changes = *changes + 1;
			size_after_encoded += 2;
		}
		size_after_encoded++;
	}
	return size_after_encoded;
}

template<typename Functor>
int percent_encode(char* buffer, std::size_t buffer_used, std::size_t buffer_len_total,
					Functor func)
{
	std::size_t changes_needed;
	std::size_t size_after_encoded = percent_encoded_size(buffer, buffer_used,
				func, &changes_needed);

	if(size_after_encoded > buffer_len_total) return -1;
	if(size_after_encoded == buffer_used) return buffer_used;

	for(std::size_t i = buffer_used - 1;; i--)
	{
		if(func(buffer[i]))
		{
				char temp[4];
				changes_needed--;
				std::snprintf(temp, 4, "%%%02X", buffer[i]);
				std::memcpy(buffer + i + 2 * changes_needed, temp, 3);
		}
		else
		{
			if(changes_needed) buffer[i + 2 * changes_needed] = buffer[i];
			else return static_cast<int>(size_after_encoded);
		}
		if(i == 0) break;
	}
	return static_cast<int>(size_after_encoded);
}


// std::size_t percent_encoded_size(const char* buffer, std::size_t buffer_used,
								// char const * const encoded_list, std::size_t encoded_list_len,
								// std::size_t* changes /* = nullptr */)
// {
	// encoder_list list(encoded_list, encoded_list_len);
	// return percent_encoded_size(buffer, buffer_used, list.list_, list.size_, changes);
// }

int percent_encode(char* buffer, std::size_t buffer_used, std::size_t buffer_len_total,
					char const* const encode_list, std::size_t encode_list_len)
{
	encoder_list list(encode_list, encode_list_len);
	return percent_encode(buffer, buffer_used, buffer_len_total, list);
}


}//Helper
}//CoAP

#endif /* COAP_TE_HELPER_ENCODE_IMPL_HPP__ */

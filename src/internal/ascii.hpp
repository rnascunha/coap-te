#ifndef COAP_TE_HELPER_ASCII_HPP__
#define COAP_TE_HELPER_ASCII_HPP__

#include <cstdint>

namespace CoAP{
namespace Helper{

inline bool is_digit(char c){ return c >= '0' && c <= '9'; }
inline bool is_lower_alpha(char c){ return c >= 'a' && c <= 'z'; }
inline bool is_upper_alpha(char c){ return c >= 'A' && c <= 'Z'; }
inline bool is_alpha(char c){ return is_lower_alpha(c) || is_upper_alpha(c); }
inline bool is_alpha_digit(char c){ return is_alpha(c) || is_digit(c); }
inline char to_lower_case(char c){ return c + ('a' - 'A'); }
inline char to_upper_case(char c){ return c - ('a' - 'A'); }
inline void array_to_lower_case(char* arr, std::size_t len)
{
	while(len--)
	{
		if(is_upper_alpha(*arr)) *arr = to_lower_case(*arr);
		arr++;
	}
}

inline void array_to_upper_case(char* arr, std::size_t len)
{
	while(len--)
	{
		if(is_lower_alpha(*arr)) *arr = to_upper_case(*arr);
		arr++;
	}
}

//https://tools.ietf.org/html/rfc3986#section-2.3
inline bool is_unreserved(char c)
{
	return is_alpha_digit(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

//https://tools.ietf.org/html/rfc3986#section-2.2
inline bool is_gen_delimits(char c)
{
	return c == ':' || c == '/' || c == '?' ||
			c == '#' || c == '[' || c == ']' || c == '@';
}

inline bool is_subgen_delimits(char c)
{
	return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' ||
			c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c == '=';
}

inline bool is_reserved(char c)
{
	return is_gen_delimits(c) || is_subgen_delimits(c);
}

}//Helper
}//CoAP

#endif /* COAP_TE_HELPER_ASCII_HPP__ */

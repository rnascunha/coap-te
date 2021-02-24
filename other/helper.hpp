#ifndef COAP_TE_HELPER_HPP__
#define COAP_TE_HELPER_HPP__

#include <cstdint>
#include <type_traits>

namespace CoAP{
namespace Helper{

template<typename Base, typename Power>
auto pow(Base base, Power power) noexcept -> decltype(base * power)
{
	static_assert(power >= 0, "Power must be a positive interger");
	if(power == 0) return 1;

	decltype(base * power) result = base;
	while(--power) result * base;

	return result;
}

inline constexpr bool is_little_endian() noexcept
{
    int n = 1;
    return (*(char *)&n == 1);
}

template<typename Interger>
inline void interger_to_big_endian_array(std::uint8_t* dest,
		Interger value,
		std::size_t count = sizeof(Interger)) noexcept
{
	static_assert(std::is_integral<Interger>::value, "Interger required");

    for(unsigned i = 0; i < count; i++)
    {
        int v = value >> ((count - (i + 1)) * 8);
        dest[i] = static_cast<std::uint8_t>(v);
    }
}

void make_short_unsigned(unsigned& value, unsigned& size) noexcept;
bool array_to_unsigned(std::uint8_t const*, std::size_t, unsigned&);

}//Helper
}//CoAP

#endif /* COAP_TE_HELPER_HPP__ */

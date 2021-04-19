#ifndef COAP_TE_INTERNAL_HELPER_HPP__
#define COAP_TE_INTERNAL_HELPER_HPP__

#include <cstdlib>
#include <cstdint>
#include <type_traits>

namespace CoAP{
namespace Helper{

template<typename Base, typename Power>
auto pow(Base base, Power power) noexcept -> decltype(base * power)
{
	if(power == 0) return 1;

	decltype(base * power) result = base;
	while(--power) result *= base;

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
        int v = static_cast<int>(value >> ((count - (i + 1)) * 8));
        dest[i] = static_cast<std::uint8_t>(v);
    }
}

void make_short_unsigned(unsigned& value, unsigned& size) noexcept;
bool array_to_unsigned(std::uint8_t const*, std::size_t, unsigned&);

void shift_left(std::uint8_t* buffer, std::size_t size, std::size_t shift) noexcept;
void shift_right(std::uint8_t* buffer, std::size_t size, std::size_t shift) noexcept;

}//Helper
}//CoAP

#endif /* COAP_TE_INTERNAL_HELPER_HPP__ */

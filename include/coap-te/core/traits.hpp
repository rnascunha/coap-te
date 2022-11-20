#ifndef COAP_TE_CORE_TRAITS_HPP
#define COAP_TE_CORE_TRAITS_HPP

#include <type_traits>
#include <iterator>

namespace coap_te {
namespace core {

template<typename Iter, typename IterCategory>
struct is_iterator_type : 
    std::bool_constant<
        std::is_convertible_v<
            typename std::iterator_traits<Iter>::iterator_category, 
            IterCategory>>
{};

template<typename Iter, typename IterCategory>
static constexpr bool is_iterator_type_v = is_iterator_type<Iter, IterCategory>::value;

template<typename Iter>
struct is_bidirectional_iterator : 
    is_iterator_type<
        Iter, 
        std::bidirectional_iterator_tag>
{};

template<typename Iter>
static constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<Iter>::value;

template<typename Iter>
struct is_random_access_iterator : 
    is_iterator_type<
        Iter, 
        std::random_access_iterator_tag>
{};

template<typename Iter>
static constexpr bool is_random_access_iterator_v = is_random_access_iterator<Iter>::value;

}//core
}//coap_te

#endif /* COAP_TE_CORE_TRAITS_HPP */
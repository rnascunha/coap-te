#ifndef COAP_TE_CORE_IMPL_UTILITY_IPP_
#define COAP_TE_CORE_IMPL_UTILITY_IPP_

namespace coap_te {
namespace core {

template<typename RandomAccessIt, typename T>
[[nodiscard]] constexpr RandomAccessIt
binary_search(RandomAccessIt begin, RandomAccessIt end,
              T const& value) noexcept {
  static_assert(is_random_access_iterator_v<RandomAccessIt>,
                "Wrong iterator type. Must be random access");
  //https://stackoverflow.com/a/44522730
  static_assert(is_equal_comparable_v<std::remove_reference_t<decltype(*begin)>, T>);

  if (begin >= end) {
    return end;
  }

  auto rend = end;
  while (begin <= end) {
    auto mid = begin + (end - begin) / 2;
    if (*mid == value)
      return mid;
    if (*mid < value)
      begin = mid + 1;
    else
      end = mid - 1;
  }
  return rend;
}

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_UTILITY_IPP_
/**
 * @file size_of.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <iostream>
#include <vector>

#include "coap-te.hpp"

int main() {
  std::cout << "\t\tSizeof types:\n\n";
  std::cout << "\t>>>>> BUFFER <<<<<\n";
  std::cout << "const_buffer:\t\t\t\t"
    << sizeof(coap_te::const_buffer) << "\n";
  std::cout << "mutable_buffer:\t\t\t\t"
    << sizeof(coap_te::mutable_buffer) << "\n";
  std::cout << "buffers_iterator<BufferSequence>:\t" <<
    sizeof(coap_te::buffers_iterator<std::vector<coap_te::const_buffer>>)
    << "\n";
  std::cout << "buffers_iterator<const_buffer>:\t\t" <<
    sizeof(coap_te::buffers_iterator<coap_te::const_buffer>)
    << "\n";
  std::cout << "buffers_iterator<mutable_buffer>:\t" <<
    sizeof(coap_te::buffers_iterator<coap_te::mutable_buffer>)
    << "\n";
  std::cout << "buffer_range<buffers_iterator>:\t\t" <<
    sizeof(coap_te::buffer_range<
      coap_te::buffers_iterator<std::vector<coap_te::const_buffer>>>)
    << "\n";
  std::cout << "buffer_range<buffers_iterator<cb>>:\t" <<
    sizeof(coap_te::buffer_range<
      coap_te::buffers_iterator<coap_te::const_buffer>>)
    << "\n";
  std::cout << "buffer_range<buffers_iterator<mb>>:\t" <<
    sizeof(coap_te::buffer_range<
      coap_te::buffers_iterator<coap_te::mutable_buffer>>)
    << "\n";

  std::cout << "\n\t>>>>> MESSAGE <<<<<\n";
  std::cout << "request:\t\t\t" << sizeof(coap_te::request) << "\n";
  std::cout << "response:\t\t\t" << sizeof(coap_te::response) << "\n";
  std::cout << "no_alloc::request:\t\t"
            << sizeof(coap_te::no_alloc::request) << "\n";

  return EXIT_SUCCESS;
}

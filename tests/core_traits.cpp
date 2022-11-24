/**
 * @file core_traits.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>

#include <forward_list>
#include <list>
#include <vector>
#include <string>
#include <string_view>
#include <array>

#include "coap-te.hpp"  //NOLINT

TEST(CoreTraits, Container) {
  // iterator category trait
  // Input
  {
    using vv = coap_te::core::is_iterator_type<
                std::forward_list<int>::iterator,
                std::input_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }
  {
    using vv = coap_te::core::is_iterator_type<
                std::list<int>::iterator,
                std::input_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }
  {
    using vv = coap_te::core::is_iterator_type<
                std::vector<int>::iterator,
                std::input_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }
  {
    [[maybe_unused]] int v[5];
    using vv = coap_te::core::is_iterator_type<
                decltype(std::begin(v)),
                std::input_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }
  // Output
  {
    using vv = coap_te::core::is_iterator_type<
                std::forward_list<int>::iterator,
                std::output_iterator_tag>;
    EXPECT_FALSE(vv::value);
  }
  {
    using vv = coap_te::core::is_iterator_type<
                std::list<int>::iterator,
                std::output_iterator_tag>;
    EXPECT_FALSE(vv::value);
  }
  {
    using vv = coap_te::core::is_iterator_type<
                std::vector<int>::iterator,
                std::output_iterator_tag>;
    EXPECT_FALSE(vv::value);
  }
  {
    [[maybe_unused]] int v[5];
    using vv = coap_te::core::is_iterator_type<
                decltype(std::begin(v)),
                std::output_iterator_tag>;
    EXPECT_FALSE(vv::value);
  }
  // Forward
  {
    using vv = coap_te::core::is_iterator_type<
                std::forward_list<int>::iterator,
                std::forward_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }
  {
    using vv = coap_te::core::is_iterator_type<
                std::list<int>::iterator,
                std::forward_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }
  {
    using vv = coap_te::core::is_iterator_type<
                std::vector<int>::iterator,
                std::forward_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }
  {
    [[maybe_unused]] int v[5];
    using vv = coap_te::core::is_iterator_type<
                decltype(std::begin(v)),
                std::forward_iterator_tag>;
    EXPECT_TRUE(vv::value);
  }

  // bidirectional trait
  EXPECT_FALSE(coap_te::core::is_bidirectional_iterator_v<
                std::forward_list<int>::iterator>);
  EXPECT_TRUE(coap_te::core::is_bidirectional_iterator_v<
                std::list<int>::iterator>);
  EXPECT_TRUE(coap_te::core::is_bidirectional_iterator_v<
                std::vector<int>::iterator>);
  {
    [[maybe_unused]] int v[5];
    EXPECT_TRUE(coap_te::core::is_bidirectional_iterator_v<
                decltype(std::begin(v))>);
  }

  // ramdom access trait
  EXPECT_FALSE(coap_te::core::is_random_access_iterator_v<
                std::forward_list<int>::iterator>);
  EXPECT_FALSE(coap_te::core::is_random_access_iterator_v<
                std::list<int>::iterator>);
  EXPECT_TRUE(coap_te::core::is_random_access_iterator_v<
                std::vector<int>::iterator>);
  {
    [[maybe_unused]] int v[5];
    EXPECT_TRUE(coap_te::core::is_random_access_iterator_v<
                decltype(std::begin(v))>);
  }
}

TEST(CoreTraits, CheckOperator) {
  {
    // Check if types are equal comparable (i.e, has operator== defined)

    // Same types
    EXPECT_TRUE(coap_te::core::is_equal_comparable_v<char>);
    EXPECT_TRUE(coap_te::core::is_equal_comparable_v<int>);
    EXPECT_TRUE(coap_te::core::is_equal_comparable_v<unsigned>);
    EXPECT_TRUE(coap_te::core::is_equal_comparable_v<long>);  //NOLINT
    EXPECT_TRUE(coap_te::core::is_equal_comparable_v<float>);
    EXPECT_TRUE(coap_te::core::is_equal_comparable_v<double>);

    // Diferent types
    {
      auto is_comp = coap_te::core::is_equal_comparable_v<char, int>;
      EXPECT_TRUE(is_comp);
    }
    {
      auto is_comp = coap_te::core::is_equal_comparable_v<int, int>;
      EXPECT_TRUE(is_comp);
    }
    {
      auto is_comp = coap_te::core::is_equal_comparable_v<unsigned, unsigned>;
      EXPECT_TRUE(is_comp);
    }
    {
      auto is_comp = coap_te::core::is_equal_comparable_v<double, long>;  // NOLINT
      EXPECT_TRUE(is_comp);
    }
    {
      auto is_comp = coap_te::core::is_equal_comparable_v<double, float>;
      EXPECT_TRUE(is_comp);
    }
    {
      auto is_comp = coap_te::core::is_equal_comparable_v<double, char>;
      EXPECT_TRUE(is_comp);
    }

    {
      struct A{};
      struct B{
        bool operator==(A) { return true; }
        bool operator==(B) { return true; }
        bool operator==(int) { return true; }
        bool operator==(float) { return true; }
      };

      EXPECT_FALSE(coap_te::core::is_equal_comparable_v<A>);
      EXPECT_TRUE(coap_te::core::is_equal_comparable_v<B>);
      {
        auto is_comp = coap_te::core::is_equal_comparable_v<B, int>;
        EXPECT_TRUE(is_comp);
      }
      {
        auto is_comp = coap_te::core::is_equal_comparable_v<int, B>;
        EXPECT_FALSE(is_comp);
      }
      {
        auto is_comp = coap_te::core::is_equal_comparable_v<B, float>;
        EXPECT_TRUE(is_comp);
      }
      {
        auto is_comp = coap_te::core::is_equal_comparable_v<float, B>;
        EXPECT_FALSE(is_comp);
      }
    }
  }
}

TEST(CoreTraits, IsBufferTrait) {
  // Check if class can be used as buffer type
  {
    auto vv = coap_te::core::is_buffer_type_v<std::array<int, 4>>;
    EXPECT_TRUE(vv);
  }
  EXPECT_TRUE(coap_te::core::is_buffer_type_v<std::vector<int>>);
  EXPECT_TRUE(coap_te::core::is_buffer_type_v<std::string>);
  EXPECT_TRUE(coap_te::core::is_buffer_type_v<std::string_view>);

  struct A{};
  struct B {
    const void* data() { return nullptr; }
  };
  struct C {
    std::size_t size() { return 0; }
  };
  struct D {
    const void* data() { return nullptr; }
    std::size_t size() { return 0; }
  };
  struct E {
    int* data() { return nullptr; }
    std::size_t size() { return 0; }
  };
  struct F {
    char data() { return '0'; }
    std::size_t size() { return 0; }
  };
  struct G {
    const char* data() { return nullptr; }
    int size() { return 0; }
  };

  EXPECT_FALSE(coap_te::core::is_buffer_type_v<A>);
  EXPECT_FALSE(coap_te::core::is_buffer_type_v<B>);
  EXPECT_FALSE(coap_te::core::is_buffer_type_v<C>);
  EXPECT_TRUE(coap_te::core::is_buffer_type_v<D>);
  EXPECT_TRUE(coap_te::core::is_buffer_type_v<E>);
  EXPECT_FALSE(coap_te::core::is_buffer_type_v<F>);
  EXPECT_FALSE(coap_te::core::is_buffer_type_v<G>);
}

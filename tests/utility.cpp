#include <forward_list>
#include <list>
#include <vector>

#include <iterator>

#include "coap-te.hpp"

#include <gtest/gtest.h>

TEST(Core, Traits) {
    //iterator category trait
    //Input
    {
        using vv = coap_te::core::is_iterator_type<std::forward_list<int>::iterator, std::input_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }
    {
        using vv = coap_te::core::is_iterator_type<std::list<int>::iterator, std::input_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }
    {
        using vv = coap_te::core::is_iterator_type<std::vector<int>::iterator, std::input_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }
    {
        [[maybe_unused]] int v[5];
        using vv = coap_te::core::is_iterator_type<decltype(std::begin(v)), std::input_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }
    //Output
    {
        using vv = coap_te::core::is_iterator_type<std::forward_list<int>::iterator, std::output_iterator_tag>;
        EXPECT_FALSE(vv::value);
    }
    {
        using vv = coap_te::core::is_iterator_type<std::list<int>::iterator, std::output_iterator_tag>;
        EXPECT_FALSE(vv::value);
    }
    {
        using vv = coap_te::core::is_iterator_type<std::vector<int>::iterator, std::output_iterator_tag>;
        EXPECT_FALSE(vv::value);
    }
    {
        [[maybe_unused]] int v[5];
        using vv = coap_te::core::is_iterator_type<decltype(std::begin(v)), std::output_iterator_tag>;
        EXPECT_FALSE(vv::value);
    }
    //Forward
    {
        using vv = coap_te::core::is_iterator_type<std::forward_list<int>::iterator, std::forward_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }
    {
        using vv = coap_te::core::is_iterator_type<std::list<int>::iterator, std::forward_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }
    {
        using vv = coap_te::core::is_iterator_type<std::vector<int>::iterator, std::forward_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }
    {
        [[maybe_unused]] int v[5];
        using vv = coap_te::core::is_iterator_type<decltype(std::begin(v)), std::forward_iterator_tag>;
        EXPECT_TRUE(vv::value);
    }

    //bidirectional trait
    EXPECT_FALSE(coap_te::core::is_bidirectional_iterator_v<std::forward_list<int>::iterator>);
    EXPECT_TRUE(coap_te::core::is_bidirectional_iterator_v<std::list<int>::iterator>);
    EXPECT_TRUE(coap_te::core::is_bidirectional_iterator_v<std::vector<int>::iterator>);
    {
        [[maybe_unused]] int v[5];
        EXPECT_TRUE(coap_te::core::is_bidirectional_iterator_v<decltype(std::begin(v))>);
    }

    //ramdom access trait
    EXPECT_FALSE(coap_te::core::is_random_access_iterator_v<std::forward_list<int>::iterator>);
    EXPECT_FALSE(coap_te::core::is_random_access_iterator_v<std::list<int>::iterator>);
    EXPECT_TRUE(coap_te::core::is_random_access_iterator_v<std::vector<int>::iterator>);
    {
        [[maybe_unused]] int v[5];
        EXPECT_TRUE(coap_te::core::is_random_access_iterator_v<decltype(std::begin(v))>);
    }
}

TEST(Core, Utility) {
    {
        const std::vector<int> v{-10, -7, -1, 1, 2, 5, 7, 9, 23};
        
        //Check if is found a element
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), -100) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), -11) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), -10) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), -9) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), -7) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), -1) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 0) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 1) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 2) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 3) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 5) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 6) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 9) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 10) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 15) == v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 23) != v.end());
        EXPECT_TRUE(coap_te::core::binary_search(v.begin(), v.end(), 24) == v.end());

        //Check if the element found is the correct one
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), -10), -10);
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), -7), -7);
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), -1), -1);
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), 1), 1);
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), 2), 2);
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), 5), 5);
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), 9), 9);
        EXPECT_EQ(*coap_te::core::binary_search(v.begin(), v.end(), 23), 23);
    }
    {
        constexpr int v[]{-10, -7, -1, 1, 2, 5, 7, 9, 23};
        
        //Check if is found a element
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -100) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -11) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -10) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -9) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -7) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -1) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 0) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 1) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 2) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 3) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 5) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 6) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 9) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 10) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 15) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 23) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 24) == std::end(v));

        //Check if the element found is the correct one
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), -10), -10);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), -7), -7);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), -1), -1);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), 1), 1);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), 2), 2);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), 5), 5);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), 9), 9);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), 23), 23);
    }
    {
        //Testing empty array
        const std::vector<int> v{};
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -100) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -11) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -10) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -9) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -7) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -1) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 0) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 1) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 2) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 3) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 5) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 6) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 9) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 10) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 15) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 23) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 24) == std::end(v));
    }
    {
        //Testing one element array
        constexpr int v[]{1};
        
        //Check if is found a element
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -100) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -11) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -10) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -9) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -7) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -1) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 0) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 1) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 2) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 3) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 5) ==std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 6) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 9) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 10) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 15) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 23) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 24) == std::end(v));

        //Check if the element found is the correct one
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), 1), 1);
    }
    {
        //Testing 2 elements array
        constexpr int v[]{-1, 1};
        
        //Check if is found a element
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -100) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -11) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -10) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -9) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -7) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), -1) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 0) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 1) != std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 2) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 3) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 5) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 6) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 9) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 10) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 15) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 23) == std::end(v));
        EXPECT_TRUE(coap_te::core::binary_search(std::begin(v), std::end(v), 24) == std::end(v));

        //Check if the element found is the correct one
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), -1), -1);
        EXPECT_EQ(*coap_te::core::binary_search(std::begin(v), std::end(v), 1), 1);
    }
}
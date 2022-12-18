/**
 * @file sorted_list.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>

#include <algorithm>

#include "coap-te/core/sorted_list.hpp"

namespace core = coap_te::core;

using list = core::sorted_list<int>;

template<typename Container>
void check_list_equality(const list& l, const Container& sorted) {
  EXPECT_EQ(l.size(), sorted.size());
  int i = 0;
  for (auto& n : l) {
    EXPECT_EQ(n, sorted[i++]);
  }
  EXPECT_TRUE(std::equal(l.begin(), l.end(), sorted.begin()));
}

void test_list_inclusion(const std::initializer_list<int>& ll) {
  list l(ll);
  EXPECT_EQ(ll.size(), l.size());
  EXPECT_FALSE(l.empty());
  EXPECT_TRUE(std::is_sorted(l.begin(), l.end()));
}

TEST(CoreSortedList, Insert) {
  // Insert
  {
    list l;
    EXPECT_TRUE(l.empty());

    l.insert(1)
      .insert(2)
      .insert(3);
    check_list_equality(l, std::vector{1, 2, 3});
  }
  {
    SCOPED_TRACE("Ordered insert");
    test_list_inclusion({1, 2, 3, 4, 5, 6, 7, 8, 9});
  }
  {
    SCOPED_TRACE("inverse ordered insert");
    test_list_inclusion({9, 8, 7, 6, 5, 4, 3, 2, 1});
  }
  {
    SCOPED_TRACE("Random insert no repeat");
    test_list_inclusion({2, 1, 7, 6, 3, 4, 5, 9, 8});
  }
  {
    SCOPED_TRACE("Random insert repeat");
    test_list_inclusion({2, 2, 2, 3, 4, 4, 4, 0, 0, 0,
                       5, 5, 1, 1, 1, -1, -1, -1});
  }
  {
    SCOPED_TRACE("Random insert repeat 2");
    test_list_inclusion({2, 1, 3, 4, 7, -2, 4, 1, 0, 0,
                       9, 7, 8, 2, 1, -2, -2, -1, 3, 4,
                       1, 7, 9, 10, 1, 7, 3});
  }
}

TEST(CoreSortedList, Get) {
  // get
  {
    list l;
    EXPECT_TRUE(l.empty());

    l.insert(1)
      .insert(2)
      .insert(3)
      .insert(1)
      .insert(2);

    auto it = l.get(1);
    EXPECT_NE(it, l.end());
    EXPECT_EQ(*it, 1);

    auto it11 = l.get(1, 1);
    EXPECT_NE(it11, l.end());
    EXPECT_EQ(*it11, 1);

    auto it12 = l.get(1, 2);
    EXPECT_EQ(it12, l.end());

    auto itu = l.get(4);
    EXPECT_EQ(itu, l.end());
  }
  {
    struct test {
      test(int v, const char* n)
        : value(v), name(n) {}
      int value;
      const char* name;

      bool operator==(const test& v) const noexcept {
        return value == v.value;
      }

      bool operator==(int v) const noexcept {
        return value == v;
      }

      bool operator<(const test& v) const noexcept {
        return value < v.value;
      }

      bool operator<(int v) const noexcept {
        return value < v;
      }
    };

    using list_test = coap_te::core::sorted_list<test>;
    list_test l;

    l.insert({1, "1"})
      .insert({1, "2"})
      .insert({1, "3"})
      .insert({2, "1"})
      .insert({2, "2"})
      .insert({3, "1"})
      .insert({3, "2"})
      .insert({3, "3"});

    std::vector<std::tuple<int, int, const char*>> vs = {
      {1, 0, "1"}, {1, 1, "2"}, {1, 2, "3"},
      {2, 0, "1"}, {2, 1, "2"},
      {3, 0, "1"}, {3, 1, "2"}, {3, 2, "3"},
    };

    ASSERT_EQ(vs.size(), l.size());

    for (auto [v, i, c] : vs) {
      auto it = l.get(v, i);
      EXPECT_NE(it, l.end());
      EXPECT_STREQ(it->name, c);
    }

    std::vector<std::tuple<int, int, const char*>> vf = {
      {4, 0, "1"}, {1, 3, "2"}, {1, 6, "3"},
      {2, 2, "1"}, {2, 4, "2"},
      {3, 3, "1"}, {4, 0, "2"}, {7, 2, "3"},
    };
    for (auto [v, i, c] : vf) {
      auto it = l.get(v, i);
      EXPECT_EQ(it, l.end());
    }
  }

  // front // pop_front
  {
    int nodes[]{1, 2, 3, 4, 5};
    list l(std::begin(nodes), std::end(nodes));
    for (auto i = std::begin(nodes); i != std::end(nodes); ++i) {
      EXPECT_EQ(l.front(), *i);
      l.pop_front();
    }
    EXPECT_TRUE(l.empty());
  }
  {
    int nodes[]{5, 4, 3, 2, 1};
    list l(std::begin(nodes), std::end(nodes));
    for (auto i = std::rbegin(nodes); i != std::rend(nodes); ++i) {
      EXPECT_EQ(l.front(), *i);
      l.pop_front();
    }
    EXPECT_TRUE(l.empty());
  }
}

void test_list_remove(std::vector<int>& ll) {    // NOLINT
  list l(ll.begin(), ll.end());

  auto size = l.size();
  EXPECT_EQ(size, ll.size());

  for (std::size_t i = 0; i < ll.size(); ++i) {
    EXPECT_TRUE(l.remove(ll[i]));
    --size;
    EXPECT_EQ(l.size(), size);
  }
  EXPECT_TRUE(l.empty());
}

void test_list_pop_front(std::vector<int>& ll) {   // NOLINT
  list l(ll.begin(), ll.end());

  std::sort(ll.begin(), ll.end());

  auto size = l.size();
  EXPECT_EQ(size, ll.size());

  for (std::size_t i = 0; i < ll.size(); ++i) {
    auto value = l.front();
    l.pop_front();
    EXPECT_EQ(value, ll[i]);
    --size;
    EXPECT_EQ(l.size(), size);
  }
  EXPECT_TRUE(l.empty());
}

TEST(CoreSortedList, Remove) {
  // Remove
  {
    SCOPED_TRACE("Remove from begin");
    std::vector<int> ll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_remove(ll);
  }
  {
    SCOPED_TRACE("Remove from end");
    std::vector<int> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    test_list_remove(ll);
  }
  {
    SCOPED_TRACE("Remove mix and repeated");
    std::vector<int> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                         1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_remove(ll);
  }
  {
    SCOPED_TRACE("Remove mix and repeated 2");
    std::vector<int> ll{10, 9, 7, 6, 7, 5, 4, 3, 2, 1,
                         1, 2, 2, 4, 4, 6, 7, 1, 0, 10};
    test_list_remove(ll);
  }
  // Pop front
  {
    SCOPED_TRACE("Pop front sorted");
    std::vector<int> ll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_pop_front(ll);
  }
  {
    SCOPED_TRACE("Pop front revert insert");
    std::vector<int> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    test_list_pop_front(ll);
  }
  {
    SCOPED_TRACE("Pop front mix and repeated");
    std::vector<int> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                         1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_pop_front(ll);
  }
  {
    SCOPED_TRACE("prop_front mix and repeated 2");
    std::vector<int> ll{10, 9, 7, 6, 7, 5, 4, 3, 2, 1,
                         1, 2, 2, 4, 4, 6, 7, 1, 0, 10};
    test_list_pop_front(ll);
  }
}

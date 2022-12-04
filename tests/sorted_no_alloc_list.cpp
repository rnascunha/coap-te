/**
 * @file sorted_no_alloc_list.cpp
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

#include "coap-te/core/sorted_no_alloc_list.hpp"

namespace core = coap_te::core;

using list = core::sorted_no_alloc_list<int>;
using node = list::node;

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
  std::vector<node> original;
  original.reserve(ll.size());

  std::vector<int> sorted;
  sorted.reserve(ll.size());
  for (auto i : ll) {
    original.emplace_back(i);
    sorted.push_back(i);
  }

  std::sort(sorted.begin(), sorted.end());

  list l;
  ASSERT_TRUE(l.empty());
  int size = 0;
  for (auto& n : original) {
    l.add(n);
    ++size;
    EXPECT_EQ(l.size(), size);
  }

  check_list_equality(l, sorted);
}

TEST(CoreSortedNoAllocList, Insert) {
  // Insert
  {
    list l;
    node n1(1);
    node n2(2);
    node n3(3);

    l.add(n1)
      .add(n2)
      .add(n3);
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

TEST(CoreSortedNoAllocList, Get) {
  // get
  {
    list l;
    node n1(1);
    node n2(2);
    node n3(3);
    node n11(1);
    node n22(2);

    l.add(n1)
      .add(n2)
      .add(n22)
      .add(n3)
      .add(n11);

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
    using list_test = core::sorted_no_alloc_list<test>;
    using node_test = list_test::node;

    list_test l;
    node_test n11(1, "1");
    node_test n12(1, "2");
    node_test n13(1, "3");
    node_test n21(2, "1");
    node_test n22(2, "2");
    node_test n31(3, "1");
    node_test n32(3, "2");
    node_test n33(3, "3");

    l.add(n31)
      .add(n11)
      .add(n32)
      .add(n12)
      .add(n21)
      .add(n13)
      .add(n22)
      .add(n33);

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

  // front
  {
    node nodes[]{1, 2, 3, 4, 5};
    list l(std::begin(nodes), std::end(nodes));
    EXPECT_TRUE(l.front());
    EXPECT_EQ(l.front(), 1);
  }
  {
    node nodes[]{5, 4, 3, 2, 1};
    list l(std::begin(nodes), std::end(nodes));
    EXPECT_TRUE(l.front());
    EXPECT_EQ(l.front(), 1);
  }
  {
    list l;
    EXPECT_FALSE(l.front());
  }
}

void test_list_remove(std::vector<node>& ll) {    // NOLINT
  list l(ll.begin(), ll.end());

  auto size = l.size();
  EXPECT_EQ(size, ll.size());

  for (std::size_t i = 0; i < ll.size(); ++i) {
    auto it = l.remove(ll[i].value);
    EXPECT_TRUE(it);
    EXPECT_EQ(*it, ll[i].value);
    --size;
    EXPECT_EQ(l.size(), size);
  }
  EXPECT_TRUE(l.empty());
}

void test_list_pop_front(std::vector<node>& ll) {   // NOLINT
  list l(ll.begin(), ll.end());

  // We need to make this copy becuase sorting the original
  // array will change the values from the list, becuause it is
  // where is stored
  std::vector<node> copy(ll);
  std::sort(copy.begin(), copy.end());

  auto size = l.size();
  EXPECT_EQ(size, ll.size());

  for (std::size_t i = 0; i < ll.size(); ++i) {
    auto it = l.pop_front();
    EXPECT_TRUE(it);
    EXPECT_EQ(*it, copy[i].value);
    --size;
    EXPECT_EQ(l.size(), size);
  }
  EXPECT_TRUE(l.empty());
}

TEST(CoreSortedNoAllocList, Remove) {
  // Remove
  {
    SCOPED_TRACE("Remove from begin");
    std::vector<node> ll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_remove(ll);
  }
  {
    SCOPED_TRACE("Remove from end");
    std::vector<node> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    test_list_remove(ll);
  }
  {
    SCOPED_TRACE("Remove mix and repeated");
    std::vector<node> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                         1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_remove(ll);
  }
  {
    SCOPED_TRACE("Remove mix and repeated 2");
    std::vector<node> ll{10, 9, 7, 6, 7, 5, 4, 3, 2, 1,
                         1, 2, 2, 4, 4, 6, 7, 1, 0, 10};
    test_list_remove(ll);
  }
  // Pop front
  {
    SCOPED_TRACE("Pop front sorted");
    std::vector<node> ll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_pop_front(ll);
  }
  {
    SCOPED_TRACE("Pop front revert insert");
    std::vector<node> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    test_list_pop_front(ll);
  }
  {
    SCOPED_TRACE("Pop front mix and repeated");
    std::vector<node> ll{10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                         1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_list_pop_front(ll);
  }
  {
    SCOPED_TRACE("prop_front mix and repeated 2");
    std::vector<node> ll{10, 9, 7, 6, 7, 5, 4, 3, 2, 1,
                         1, 2, 2, 4, 4, 6, 7, 1, 0, 10};
    test_list_pop_front(ll);
  }
}

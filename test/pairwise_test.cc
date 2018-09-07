#include "catch.hpp"

#include "utl/pairwise.h"

TEST_CASE("pairwise") {
  struct obj {
    obj() = default;
    obj(int payload) : payload_{payload} {}

    obj(const obj&) = delete;
    obj(obj&&) = delete;
    obj& operator=(const obj&) = delete;
    obj& operator=(obj&&) = delete;
    virtual ~obj() = default;

    int payload_;
  };

  auto const consume_pairwise = [](auto const& range) {
    std::string result;
    for (auto const pair : range) {
      result.append(std::to_string(pair.first.payload_));
      result.push_back(',');
      result.append(std::to_string(pair.second.payload_));
      result.push_back(';');
    }
    return result;
  };

  {
    std::array<obj, 0> sut;
    std::string const expected = "";
    auto const actual = consume_pairwise(utl::pairwise(sut));
    CHECK(expected == actual);
  }

  {
    std::array<obj, 1> sut{{1}};
    std::string const expected = "";
    auto const actual = consume_pairwise(utl::pairwise(sut));
    CHECK(expected == actual);
  }

  {
    std::array<obj, 2> sut{{1, 2}};
    std::string const expected = "1,2;";
    auto const actual = consume_pairwise(utl::pairwise(sut));
    CHECK(expected == actual);
  }

  {
    std::array<obj, 3> sut{{1, 2, 3}};
    std::string const expected = "1,2;2,3;";
    auto const actual = consume_pairwise(utl::pairwise(sut));
    CHECK(expected == actual);
  }

  {
    std::array<obj, 4> sut{{1, 2, 3, 4}};
    std::string const expected = "1,2;2,3;3,4;";
    auto const actual = consume_pairwise(utl::pairwise(sut));
    CHECK(expected == actual);
  }
}

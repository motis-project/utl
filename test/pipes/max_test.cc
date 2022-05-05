#include "catch2/catch.hpp"

#include "utl/pipes.h"

using namespace utl;

TEST_CASE("max test int") {
  std::vector<int> v = {1, 3, 5, 7, 9};
  CHECK((all(v) | max<int>()) == 9);
}

TEST_CASE("max test int 2") {
  std::vector<int> v = {5, 7, 9, 1, 4};
  CHECK((all(v) | max<int>()) == 9);
}

TEST_CASE("max test float") {
  std::vector<double> v = {1.2, 3.5, 5.5, 7.3, 9.8};
  CHECK((all(v) | max<double>()) == 9.8);
}

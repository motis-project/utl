#include "../catch.hpp"

#include "utl/parser/csv.h"

using namespace utl;

cstr csv_file = R"(index,width,height,weight,comment,enabled
0,77,2,55.2,"this is real!",true
1,78,3,56.9,"hello",false
2,78,3,57.0,"world",true
)";

cstr csv_file_semicolon = R"(index;width;height;weight;comment;enabled
0;77;2;55.2;"this is real!";true
1;78;3;56.9;"hello";false
2;78;3;57.0;"world";true
)";

TEST_CASE("simple_csv") {
  typedef std::tuple<int, double, bool> entry;
  std::vector<entry> entries;
  read(csv_file, entries, {{"index", "weight", "enabled"}});

  REQUIRE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  int i = 0;
  for (auto const& r : entries) {
    REQUIRE(std::get<0>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<2>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("simple_csv_semicolon_seperator") {
  typedef std::tuple<int, double, bool> entry;
  std::vector<entry> entries;
  read<entry, ';'>(csv_file_semicolon, entries,
                   {{"index", "weight", "enabled"}});

  REQUIRE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  int i = 0;
  for (auto const& r : entries) {
    REQUIRE(std::get<0>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<2>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("wrong_order") {
  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(csv_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  int i = 0;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("missing_selected_column") {
  cstr my_file = R"(width,height,weight,comment,enabled
77,2,55.2,"this is real!",true
78,3,56.9,"hello",false
78,3,57.0,"world",true
)";

  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  int i = 0;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("additional_column") {
  cstr my_file = R"(width,height,weight,comment,enabled
77,2,55.2,"this is real!",true,123
78,3,56.9,"hello",false,123
78,3,57.0,"world",true,123
)";

  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  int i = 0;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("missing_column_bad_format") {
  cstr my_file = R"(width,height,weight,comment,enabled
77,2,55.2,"this is real!"
78,3,56.9,"hello"
78,3,57.0,"world"
)";

  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{false, false, false}};

  int i = 0;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("doubles") {
  cstr my_file = R"(SENDER;RECEIVER_ID;TIME
V1;0;0.9
V2;-1;0.1
V3;1;0.1
V4;0;0.2
V5;3;0.1
V6;1;0.1)";

  using dist = std::tuple<std::string, int, double>;
  static const utl::column_mapping<dist> columns = {
      {"SENDER", "RECEIVER_ID", "TIME"}};

  std::vector<dist> dists;
  utl::read<dist, ';'>(my_file, dists, columns);

  std::vector<dist> check_dists = {{dist{"V1", 0, 0.9}, dist{"V2", -1, 0.1},
                                    dist{"V3", 1, 0.1}, dist{"V4", 0, 0.2},
                                    dist{"V5", 3, 0.1}, dist{"V6", 1, 0.1}}};

  REQUIRE(check_dists == dists);
}

TEST_CASE("empty") {
  cstr my_file = R"(A:B:C
::
::
::
1:2:3
hello:77:world)";

  using data = std::tuple<cstr, int, cstr>;
  static const utl::column_mapping<data> columns = {{"A", "B", "C"}};

  std::vector<data> dists;
  utl::read<data, ':'>(my_file, dists, columns);

  std::vector<data> check_data = {{data{"", 0, ""}, data{"", 0, ""},
                                   data{"", 0, ""}, data{"1", 2, "3"},
                                   data{"hello", 77, "world"}}};

  REQUIRE(check_data == dists);
}

#include "../catch.hpp"

#include "utl/parser/csv.h"

namespace utl {
namespace csv_test {

static cstr csv_file =
    "index,width,height,weight,comment,enabled\r\n"
    "0,77,2,55.2,\"this is real!\",true\r\n"
    "1,78,3,56.9,\"hello\",false\r\n"
    "2,78,3,57.0,\"world\",true\r\n"
    "\r\n";

static cstr csv_file_semicolon =
    "index;width;height;weight;comment;enabled\r\n"
    "0;77;2;55.2;\"this is real!\";true\r\n"
    "1;78;3;56.9;\"hello\";false\r\n"
    "2;78;3;57.0;\"world\";true\r\n"
    "\r\n";

TEST_CASE("simple_csv_cr") {
  typedef std::tuple<unsigned, double, bool> entry;
  std::vector<entry> entries;
  read(csv_file, entries, {{"index", "weight", "enabled"}});

  REQUIRE(entries.size() == 3);

  std::array<unsigned, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0u;
  for (auto const& r : entries) {
    REQUIRE(std::get<0>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<2>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("simple_csv_semicolon_seperator_cr") {
  typedef std::tuple<unsigned, double, bool> entry;
  std::vector<entry> entries;
  read<entry, ';'>(csv_file_semicolon, entries,
                   {{"index", "weight", "enabled"}});

  REQUIRE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0u;
  for (auto const& r : entries) {
    REQUIRE(std::get<0>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<2>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("wrong_order_cr") {
  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(csv_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<unsigned, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0u;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("missing_selected_column_cr") {
  cstr my_file =
      "width,height,weight,comment,enabled\r\n"
      "77,2,55.2,\"this is real!\",true\r\n"
      "78,3,56.9,\"hello\",false\r\n"
      "78,3,57.0,\"world\",true\r\n"
      "\r\n";

  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<unsigned, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0u;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("additional_column_cr") {
  cstr my_file =
      "width,height,weight,comment,enabled\r\n"
      "77,2,55.2,\"this is real!\",true,123\r\n"
      "78,3,56.9,\"hello\",false,123\r\n"
      "78,3,57.0,\"world\",true,123\r\n"
      "\r\n";

  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<unsigned, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0u;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("missing_column_bad_format_cr") {
  cstr my_file =
      "width,height,weight,comment,enabled\r\n"
      "77,2,55.2,\"this is real!\"\r\n"
      "78,3,56.9,\"hello\"\r\n"
      "78,3,57.0,\"world\"\r\n"
      "\r\n";

  typedef std::tuple<bool, double, int> entry;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  REQUIRE(entries.size() == 3);

  std::array<unsigned, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{false, false, false}};

  auto i = 0u;
  for (auto const& r : entries) {
    REQUIRE(std::get<2>(r) == indices[i]);
    REQUIRE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    REQUIRE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST_CASE("doubles_cr") {
  cstr my_file =
      "SENDER;RECEIVER_ID;TIME\r\n"
      "V1;0;0.9\r\n"
      "V2;-1;0.1\r\n"
      "V3;1;0.1\r\n"
      "V4;0;0.2\r\n"
      "V5;3;0.1\r\n"
      "V6;1;0.1\r\n";

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

TEST_CASE("empty_cr") {
  cstr my_file =
      "A:B:C\r\n"
      "::\r\n"
      "::\r\n"
      "::\r\n"
      "1:2:3\r\n"
      "hello:77:world\r\n";

  using data = std::tuple<cstr, int, cstr>;
  static const utl::column_mapping<data> columns = {{"A", "B", "C"}};

  std::vector<data> dists;
  utl::read<data, ':'>(my_file, dists, columns);

  std::vector<data> check_data = {{data{"", 0, ""}, data{"", 0, ""},
                                   data{"", 0, ""}, data{"1", 2, "3"},
                                   data{"hello", 77, "world"}}};

  REQUIRE(check_data == dists);
}

}  // namespace csv_test
}  // namespace utl
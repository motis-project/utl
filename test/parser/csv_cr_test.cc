#include "gtest/gtest.h"

#include "utl/parser/csv.h"

namespace utl::csv_test {

cstr const csv_file =
    "index,width,height,weight,comment,enabled\r\n"
    "0,77,2,55.2,\"this is real!\",true\r\n"
    "1,78,3,56.9,\"hello\",false\r\n"
    "2,78,3,57.0,\"world\",true\r\n"
    "\r\n";

cstr const csv_file_semicolon =
    "index;width;height;weight;comment;enabled\r\n"
    "0;77;2;55.2;\"this is real!\";true\r\n"
    "1;78;3;56.9;\"hello\";false\r\n"
    "2;78;3;57.0;\"world\";true\r\n"
    "\r\n";

TEST(csv_cr_test, simple_csv_cr) {
  using entry = std::tuple<unsigned, double, bool>;
  std::vector<entry> entries;
  read(csv_file, entries, {{"index", "weight", "enabled"}});

  ASSERT_TRUE(entries.size() == 3);

  std::array<unsigned, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0U;
  for (auto const& r : entries) {
    ASSERT_TRUE(std::get<0>(r) == indices[i]);
    ASSERT_TRUE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    ASSERT_TRUE(std::get<2>(r) == enabled[i]);
    ++i;
  }
}

TEST(csv_cr_test, simple_csv_semicolon_seperator_cr) {
  using entry = std::tuple<unsigned, double, bool>;
  std::vector<entry> entries;
  read<entry, ';'>(csv_file_semicolon, entries,
                   {{"index", "weight", "enabled"}});

  ASSERT_TRUE(entries.size() == 3);

  std::array<unsigned, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0U;
  for (auto const& r : entries) {
    ASSERT_TRUE(std::get<0>(r) == indices[i]);
    ASSERT_TRUE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    ASSERT_TRUE(std::get<2>(r) == enabled[i]);
    ++i;
  }
}

TEST(csv_cr_test, wrong_order_cr) {
  using entry = std::tuple<bool, double, int>;
  std::vector<entry> entries;
  read(csv_file, entries, {{"enabled", "weight", "index"}});

  ASSERT_TRUE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 1, 2}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0U;
  for (auto const& r : entries) {
    ASSERT_TRUE(std::get<2>(r) == indices[i]);
    ASSERT_TRUE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    ASSERT_TRUE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST(csv_cr_test, missing_selected_column_cr) {
  cstr my_file =
      "width,height,weight,comment,enabled\r\n"
      "77,2,55.2,\"this is real!\",true\r\n"
      "78,3,56.9,\"hello\",false\r\n"
      "78,3,57.0,\"world\",true\r\n"
      "\r\n";

  using entry = std::tuple<bool, double, int>;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  ASSERT_TRUE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0U;
  for (auto const& r : entries) {
    ASSERT_TRUE(std::get<2>(r) == indices[i]);
    ASSERT_TRUE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    ASSERT_TRUE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST(csv_cr_test, additional_column_cr) {
  cstr my_file =
      "width,height,weight,comment,enabled\r\n"
      "77,2,55.2,\"this is real!\",true,123\r\n"
      "78,3,56.9,\"hello\",false,123\r\n"
      "78,3,57.0,\"world\",true,123\r\n"
      "\r\n";

  using entry = std::tuple<bool, double, int>;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  ASSERT_TRUE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{true, false, true}};

  auto i = 0U;
  for (auto const& r : entries) {
    ASSERT_TRUE(std::get<2>(r) == indices[i]);
    ASSERT_TRUE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    ASSERT_TRUE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST(csv_cr_test, missing_column_bad_format_cr) {
  cstr my_file =
      "width,height,weight,comment,enabled\r\n"
      "77,2,55.2,\"this is real!\"\r\n"
      "78,3,56.9,\"hello\"\r\n"
      "78,3,57.0,\"world\"\r\n"
      "\r\n";

  using entry = std::tuple<bool, double, int>;
  std::vector<entry> entries;
  read(my_file, entries, {{"enabled", "weight", "index"}});

  ASSERT_TRUE(entries.size() == 3);

  std::array<int, 3> indices = {{0, 0, 0}};
  std::array<double, 3> weights = {{55.2, 56.9, 57}};
  std::array<bool, 3> enabled = {{false, false, false}};

  auto i = 0U;
  for (auto const& r : entries) {
    ASSERT_TRUE(std::get<2>(r) == indices[i]);
    ASSERT_TRUE(std::abs(std::get<1>(r) - weights[i]) < 0.001);
    ASSERT_TRUE(std::get<0>(r) == enabled[i]);
    ++i;
  }
}

TEST(csv_cr_test, doubles_cr) {
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

  ASSERT_TRUE(check_dists == dists);
}

TEST(csv_cr_test, empty_cr) {
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

  ASSERT_TRUE(check_data == dists);
}

}  // namespace utl::csv_test

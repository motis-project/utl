#include "gtest/gtest.h"

#include "utl/parser/buf_reader.h"
#include "utl/parser/csv_range.h"
#include "utl/parser/line_range.h"
#include "utl/pipes/avg.h"
#include "utl/pipes/remove_if.h"
#include "utl/pipes/transform.h"
#include "utl/pipes/vec.h"

using namespace utl;

constexpr auto const input = R"(date,time,open,high,low,close,volume
01/02/1998,09:30,39.04,39.04,39.04,39.04,179332
01/02/1998,09:31,39,39.04,39,39,28907
01/02/1998,09:32,39.02,39.04,39,39.04,35599
01/02/1998,09:33,39.02,39.04,39,39,44967
01/02/1998,09:34,39,39.04,39,39,12850
01/02/1998,09:35,39.02,39.04,39,39,62365
01/02/1998,09:36,39.02,39.02,39,39.02,6959
01/02/1998,09:37,39,39,39,39,7494
01/02/1998,09:38,39,39.02,39,39.02,29713)";

struct quote {
  csv_col<float, UTL_NAME("open")> open_;
  csv_col<float, UTL_NAME("high")> high_;
  csv_col<float, UTL_NAME("low")> low_;
  csv_col<float, UTL_NAME("close")> close_;
  csv_col<int, UTL_NAME("volume")> volume_;
  csv_col<cstr, UTL_NAME("date")> date_;
  csv_col<cstr, UTL_NAME("time")> time_;
};

TEST(pipe_csv, csv) {
  auto const avg_volume =
      line_range{buf_reader{input, {}}}  //
      | csv<quote>()  //
      | remove_if([](auto&& row) { return row.open_ < 39.01; })  //
      | transform([](auto&& row) { return row.volume_; })  //
      | avg();
  EXPECT_TRUE(avg_volume <= 65844.5);
  EXPECT_TRUE(avg_volume >= 65844.3);
}

TEST(pipe_csv, csv_no_rows) {
  struct baz {
    csv_col<int, UTL_NAME("FOO")> foo_;
    csv_col<int, UTL_NAME("BAR")> bar_;
  };

  {
    constexpr auto const no_rows_input = R"(FOO,BAR)";
    auto const result = line_range{make_buf_reader(no_rows_input, {})}  //
                        | csv<baz>()  //
                        | vec();

    EXPECT_TRUE(result.empty() == true);
  }
  {
    constexpr auto const no_rows_input = R"(FOO,BAR

)";
    auto const result = line_range{make_buf_reader(no_rows_input, {})}  //
                        | csv<baz>()  //
                        | vec();

    EXPECT_TRUE(result.empty() == true);
  }
}

TEST(pipe_csv, csv_separator) {
  struct baz {
    csv_col<int, UTL_NAME("FOO")> foo_;
    csv_col<int, UTL_NAME("BAR")> bar_;
  };

  constexpr auto const no_rows_input = R"(BAR$FOO
1$2
)";
  auto const result = line_range{make_buf_reader(no_rows_input, {})}  //
                      | csv<baz, '$'>()  //
                      | vec();

  ASSERT_TRUE(result.size() == 1);
  EXPECT_TRUE(result[0].foo_.val() == 2);
  EXPECT_TRUE(result[0].bar_.val() == 1);
}

TEST(pipe_csv, csv_escaped_string) {
  struct dat {
    csv_col<std::string, UTL_NAME("FOO")> foo_;
    csv_col<std::string, UTL_NAME("BAR")> bar_;
    csv_col<std::string, UTL_NAME("BAZ")> baz_;
  };

  constexpr auto const input = R"(BAR,FOO,BAZ
"asd","[""asd"", ""bsd""]","xxx"
)";
  auto const result = line_range{make_buf_reader(input, {})}  //
                      | csv<dat, ','>()  //
                      | vec();

  ASSERT_TRUE(result.size() == 1);
  EXPECT_TRUE(result[0].foo_.val() == R"(["asd", "bsd"])");
  EXPECT_TRUE(result[0].bar_.val() == "asd");
  EXPECT_TRUE(result[0].baz_.val() == "xxx");
}

TEST(pipe_csv, csv_invalid_escaped_string) {
  struct dat {
    csv_col<std::string, UTL_NAME("FOO")> foo_;
    csv_col<std::string, UTL_NAME("BAR")> bar_;
    csv_col<std::string, UTL_NAME("BAZ")> baz_;
  };

  // This is invalid, but we need to make sure not to crash
  constexpr auto const input = R"(BAR,FOO,BAZ
"asd","[""asd"", ""bsd""]","xxx""
)";
  auto const result = line_range{make_buf_reader(input, {})}  //
                      | csv<dat, ','>()  //
                      | vec();

  ASSERT_TRUE(result.size() == 1);
  EXPECT_TRUE(result[0].foo_.val() == R"(["asd", "bsd"])");
  EXPECT_TRUE(result[0].bar_.val() == "asd");
  EXPECT_TRUE(result[0].baz_.val() == R"(xxx")");
}

#include "gtest/gtest.h"

#include "create_table.h"

TEST(sql, create_table) {
  struct row {
    sql_col<int, name("id"), primary, not_null> user_id_ = 0;
    sql_col<std::string, name("firstName"), not_null> first_name_;
    sql_col<std::string, name("lastName"), not_null> last_name_;
  } r;
  EXPECT_TRUE(create_table_statement(r) == R"(CREATE TABLE (
  id INT PRIMARY NOT NULL,
  firstName TEXT NOT NULL,
  lastName TEXT NOT NULL
);)");
}

#include "../catch.hpp"

#include "create_table.h"

TEST_CASE("create_table") {
  struct row {
    sql_col<int, name("id"), primary, not_null> user_id = 0;
    sql_col<std::string, name("firstName"), not_null> first_name;
    sql_col<std::string, name("lastName"), not_null> last_name;
  } r;
  CHECK(create_table_statement(r) == R"(CREATE TABLE (
  id INT PRIMARY NOT NULL,
  firstName TEXT NOT NULL,
  lastName TEXT NOT NULL
);)");
}

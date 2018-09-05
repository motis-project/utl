#include "utl/serialization/serialization.h"

#include "../catch.hpp"

TEST_CASE("struct serialization") {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      utl::string d_;
    } j;
  };

  utl::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, utl::string{"testtes"}}};
    buf = utl::serialize(obj);
  }  // EOL obj

  CHECK(buf.size() ==
        sizeof(serialize_me));  // short string, no additional space

  auto const serialized = utl::deserialize<serialize_me>(&buf[0]);
  CHECK(serialized->a_ == 1);
  CHECK(serialized->j.b_ == 2);
  CHECK(serialized->j.c_ == 3);
  CHECK(serialized->j.d_ == utl::string{"testtes"});
}
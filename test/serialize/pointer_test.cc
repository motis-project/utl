#include "utl/serialization/serialization.h"

#include "../catch.hpp"

TEST_CASE("pointer serialization") {
  struct serialize_me {
    utl::unique_ptr<int> i_{utl::make_unique<int>(77)};
    int* raw_{i_.get()};
  };

  utl::byte_buf buf;

  {
    serialize_me obj;
    buf = utl::serialize(obj);
  }  // EOL obj

  auto const serialized =
      utl::deserialize<serialize_me>(&buf[0], &buf[0] + buf.size());
  CHECK(serialized->raw_ == serialized->i_.get());
  CHECK(*serialized->raw_ == 77);
  CHECK(*serialized->i_.get() == 77);
}
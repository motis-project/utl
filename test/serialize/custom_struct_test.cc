#include "../catch.hpp"

#include "utl/serialization/serialization.h"

struct serialize_me {
  uint32_t v1_ : 10;
  uint32_t v2_ : 10;
  uint32_t v3_ : 10;
  uint32_t v4_ : 1;
  uint32_t v5_ : 1;
};

template <typename Ctx>
void serialize(Ctx&, serialize_me const*, utl::offset_t const) {}

void deserialize(utl::deserialization_context const&, serialize_me*) {}

TEST_CASE("custom struct test") {
  utl::byte_buf buf;

  {
    serialize_me obj{1, 2, 3, 0, 1};
    buf = utl::serialize(obj);
  }  // EOL obj

  auto const serialized =
      utl::deserialize<serialize_me>(&buf[0], &buf[0] + buf.size());
  CHECK(1 == serialized->v1_);
  CHECK(2 == serialized->v2_);
  CHECK(3 == serialized->v3_);
  CHECK(0 == serialized->v4_);
  CHECK(1 == serialized->v5_);
}
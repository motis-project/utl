#include "catch2/catch.hpp"

#include "utl/struct/comparable.h"
#include "utl/struct/printable.h"

#include "utl/progress_tracker.h"

struct log_entry {
  MAKE_COMPARABLE();
  MAKE_PRINTABLE(log_entry);

  bool active_;
  float out_;
  std::string msg_;
};

TEST_CASE("progress_tracker") {
  SECTION("msg") {
    std::vector<log_entry> log;
    utl::progress_tracker sut{[&log](auto& t) {
      log.push_back({t.active_, t.out_, t.msg_});
    }};

    sut.set_msg("Hello World!");
    sut.activate("GO");
    sut.deactivate("STOP");

    REQUIRE(log.size() == 3);
    CHECK(log.at(0) == log_entry{true, 0.F, "Hello World!"});
    CHECK(log.at(1) == log_entry{true, 0.F, "GO"});
    CHECK(log.at(2) == log_entry{false, 0.F, "STOP"});
  }

  SECTION("progress") {
    std::vector<log_entry> log;
    utl::progress_tracker sut{[&log](auto& t) {
      log.push_back({t.active_, t.out_, t.msg_});
    }};

    sut.set_bounds(30.F, 50.F, 2.F, 100ULL);

    for (auto i = 0ULL; i <= 100ULL; ++i) {
      sut.increment();
    }

    REQUIRE(log.size() == 12);
    CHECK(log.at(0) == log_entry{true, 0.F, ""});
    for (auto i = 0; i < 11; ++i) {
      CHECK(log.at(i + 1) == log_entry{true, 30.F + 2 * i, ""});
    }
  }
}

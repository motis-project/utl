#include "catch2/catch.hpp"

#include <iostream>
#include <sstream>

#include "utl/raii.h"

#include "cista/reflection/comparable.h"
#include "cista/reflection/printable.h"

#include "utl/progress_tracker.h"

struct log_entry {
  CISTA_COMPARABLE()
  CISTA_PRINTABLE(log_entry);

  bool show_progress_;
  float out_;
  std::string status_;
};

template <typename Fn>
std::string capture_cout(Fn&& fn) {
  auto const reset =
      utl::make_raii(std::cout.rdbuf(), [](auto* b) { std::cout.rdbuf(b); });

  std::stringstream ss;
  std::cout.rdbuf(ss.rdbuf());

  fn();

  std::cout << std::flush;
  return ss.str();
}

TEST_CASE("progress_tracker") {
  SECTION("msg") {
    std::vector<log_entry> log;
    utl::progress_tracker sut{[&](auto& t) {
      log.push_back({t.show_progress_, t.out_, t.status_});
    }};

    sut.status("Hello World!");
    sut.status("GO").show_progress(true);
    sut.status("STOP").show_progress(false);

    REQUIRE(log.size() == 3);
    CHECK(log.at(0) == log_entry{true, 0.F, "Hello World!"});
    CHECK(log.at(1) == log_entry{true, 0.F, "GO"});
    CHECK(log.at(2) == log_entry{false, 0.F, "STOP"});
  }

  SECTION("progress") {
    std::vector<log_entry> log;
    utl::progress_tracker sut{[&log](auto& t) {
      log.push_back({t.show_progress_, t.out_, t.status_});
    }};

    sut.out_bounds(30.F, 50.F).out_mod(2.F).in_high(100ULL);

    for (auto i = 0ULL; i <= 100ULL; ++i) {
      sut.increment();
    }

    REQUIRE(log.size() == 11);
    for (auto i = 0; i < 11; ++i) {
      CHECK(log.at(i) == log_entry{true, 30.F + 2 * i, ""});
    }
  }

  SECTION("in_high_zero") {
    std::vector<log_entry> log;
    utl::progress_tracker sut{[&log](auto& t) {
      log.push_back({t.show_progress_, t.out_, t.status_});
    }};

    sut.out_bounds(30.F, 50.F).in_high(0ULL);

    REQUIRE(log.size() == 1);
    CHECK(log.at(0) == log_entry{true, 30.F, ""});
    log.clear();

    sut.increment();
    REQUIRE(log.size() == 1);
    CHECK(log.at(0) == log_entry{true, 50.F, ""});
    log.clear();

    sut.increment();
    CHECK(log.empty());
  }

  SECTION("out_range_zero") {
    std::vector<log_entry> log;
    utl::progress_tracker sut{[&log](auto& t) {
      log.push_back({t.show_progress_, t.out_, t.status_});
    }};

    sut.out_bounds(30.F, 30.F);

    REQUIRE(log.size() == 1);
    CHECK(log.at(0) == log_entry{true, 30.F, ""});
    log.clear();

    sut.increment();
    CHECK(log.empty());
  }
}

#define RE_ANY "(?:.|\r|\n)*?"

TEST_CASE("global_progress_tracker") {
  SECTION("msg") {
    auto t1 = utl::get_global_progress_trackers().get_tracker("module_1");
    auto t2 = utl::get_global_progress_trackers().get_tracker("module_2");

    auto const str = capture_cout([&] {
      auto const progress_bars = utl::global_progress_bars{};
      t1->status("WAITING");
      t2->status("READY");
    });

    CHECK_THAT(str, Catch::Matches(RE_ANY "module_1.*?WAITING" RE_ANY));
    CHECK_THAT(str, Catch::Matches(RE_ANY "module_2.*?READY" RE_ANY));
  }

  SECTION("silent and clear") {
    auto t1 = utl::get_global_progress_trackers().get_tracker("module_1");

    auto const str1 = capture_cout([&] { t1->status("ASDF"); });
    CHECK(str1.empty());

    utl::get_global_progress_trackers().clear();
    auto const str2 =
        capture_cout([&] { utl::get_global_progress_trackers().print(); });
    CHECK(str2.empty());
  }

  SECTION("progress") {
    auto t1 = utl::get_global_progress_trackers().get_tracker("module_1");
    utl::get_global_progress_trackers().get_tracker("module_2");

    auto const str = capture_cout([&] {
      auto const progress_bars = utl::global_progress_bars{};
      t1->update(50ULL);
    });

    CHECK_THAT(str, Catch::Matches(RE_ANY "module_1.*?50%" RE_ANY));
    CHECK_THAT(str, Catch::Matches(RE_ANY "module_2.*?0%" RE_ANY));
  }
}

TEST_CASE("active_progress_tracker") {
  SECTION("one") {
    auto const str = capture_cout([&] {
      auto const progress_bars = utl::global_progress_bars{};
      utl::activate_progress_tracker("first")->status("YEAH");
      utl::get_active_progress_tracker()->status("ASDF");
    });

    CHECK_THAT(str, Catch::Matches(RE_ANY "first.*?YEAH" RE_ANY));
    CHECK_THAT(str, Catch::Matches(RE_ANY "first.*?ASDF" RE_ANY));
  }

  SECTION("two") {
    auto const str = capture_cout([&] {
      auto const progress_bars = utl::global_progress_bars{};
      utl::activate_progress_tracker("second");
      utl::get_active_progress_tracker()->status("QWERTZ");
    });

    CHECK_THAT(str, Catch::Matches(RE_ANY "second.*?QWERTZ" RE_ANY));
  }

  SECTION("clear") {
    utl::activate_progress_tracker("third");
    utl::get_active_progress_tracker()->status("ASDF");

    utl::get_global_progress_trackers().clear();
    CHECK_THROWS(utl::get_active_progress_tracker());
  }

  SECTION("use after clear") {
    auto tracker = utl::activate_progress_tracker("fourth");
    utl::get_global_progress_trackers().clear();

    auto const str = capture_cout([&] { tracker->status("detached tracker"); });

    CHECK(str.empty());
    CHECK(tracker->status_ == "detached tracker");
  }
}

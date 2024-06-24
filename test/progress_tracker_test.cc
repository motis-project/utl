#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

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

TEST(progress_tracker_progress_tracker, msg) {
  std::vector<log_entry> log;
  utl::progress_tracker sut{
      [&](auto& t) { log.push_back({t.show_progress_, t.out_, t.status_}); }};

  sut.status("Hello World!");
  sut.status("GO").show_progress(true);
  sut.status("STOP").show_progress(false);

  ASSERT_TRUE(log.size() == 3);
  EXPECT_TRUE(log.at(0) == (log_entry{true, 0.F, "Hello World!"}));
  EXPECT_TRUE(log.at(1) == (log_entry{true, 0.F, "GO"}));
  EXPECT_TRUE(log.at(2) == (log_entry{false, 0.F, "STOP"}));
}

TEST(progress_tracker_progress_tracker, progress) {
  std::vector<log_entry> log;
  utl::progress_tracker sut{[&log](auto& t) {
    log.push_back({t.show_progress_, t.out_, t.status_});
  }};

  sut.out_bounds(30.F, 50.F).out_mod(2.F).in_high(100ULL);

  for (auto i = 0ULL; i <= 100ULL; ++i) {
    sut.increment();
  }

  ASSERT_TRUE(log.size() == 11);
  for (auto i = 0; i < 11; ++i) {
    EXPECT_TRUE(log.at(i) == (log_entry{true, 30.F + 2 * i, ""}));
  }
}

TEST(progress_tracker_progress_tracker, in_high_zero) {
  std::vector<log_entry> log;
  utl::progress_tracker sut{[&log](auto& t) {
    log.push_back({t.show_progress_, t.out_, t.status_});
  }};

  sut.out_bounds(30.F, 50.F).in_high(0ULL);

  ASSERT_TRUE(log.size() == 1);
  EXPECT_TRUE(log.at(0) == (log_entry{true, 30.F, ""}));
  log.clear();

  sut.increment();
  ASSERT_TRUE(log.size() == 1);
  EXPECT_TRUE(log.at(0) == (log_entry{true, 50.F, ""}));
  log.clear();

  sut.increment();
  EXPECT_TRUE(log.empty());
}

TEST(progress_tracker_progress_tracker, out_range_zero) {
  std::vector<log_entry> log;
  utl::progress_tracker sut{[&log](auto& t) {
    log.push_back({t.show_progress_, t.out_, t.status_});
  }};

  sut.out_bounds(30.F, 30.F);

  ASSERT_TRUE(log.size() == 1);
  EXPECT_TRUE(log.at(0) == (log_entry{true, 30.F, ""}));
  log.clear();

  sut.increment();
  EXPECT_TRUE(log.empty());
}

TEST(progress_tracker_global_progress_tracker, msg) {
  auto t1 = utl::get_global_progress_trackers().get_tracker("module_1");
  auto t2 = utl::get_global_progress_trackers().get_tracker("module_2");

  auto const str = capture_cout([&] {
    auto const progress_bars = utl::global_progress_bars{};
    t1->status("WAITING");
    t2->status("READY");
  });

  EXPECT_THAT(str, testing::ContainsRegex("module_1.*WAITING"));
  EXPECT_THAT(str, testing::ContainsRegex("module_2.*READY"));
}

TEST(progress_tracker_global_progress_tracker, silent_and_clear) {
  auto t1 = utl::get_global_progress_trackers().get_tracker("module_1");

  auto const str1 = capture_cout([&] { t1->status("ASDF"); });
  EXPECT_TRUE(str1.empty());

  utl::get_global_progress_trackers().clear();
  auto const str2 =
      capture_cout([&] { utl::get_global_progress_trackers().print(); });
  EXPECT_TRUE(str2.empty());
}

TEST(progress_tracker_global_progress_tracker, progress) {
  auto t1 = utl::get_global_progress_trackers().get_tracker("module_1");
  utl::get_global_progress_trackers().get_tracker("module_2");

  auto const str = capture_cout([&] {
    auto const progress_bars = utl::global_progress_bars{};
    t1->update(50ULL);
  });

  EXPECT_THAT(str, testing::ContainsRegex("module_1.*50%"));
  EXPECT_THAT(str, testing::ContainsRegex("module_2.*0%"));
}

TEST(progress_tracker_active_progress_tracker, one) {
  auto const str = capture_cout([&] {
    auto const progress_bars = utl::global_progress_bars{};
    utl::activate_progress_tracker("first")->status("YEAH");
    utl::get_active_progress_tracker()->status("ASDF");
  });

  EXPECT_THAT(str, testing::ContainsRegex("first.*YEAH"));
  EXPECT_THAT(str, testing::ContainsRegex("first.*ASDF"));
}

TEST(progress_tracker_active_progress_tracker, two) {
  auto const str = capture_cout([&] {
    auto const progress_bars = utl::global_progress_bars{};
    utl::activate_progress_tracker("second");
    utl::get_active_progress_tracker()->status("QWERTZ");
  });

  EXPECT_THAT(str, testing::ContainsRegex("second.*QWERTZ"));
}

TEST(progress_tracker_active_progress_tracker, clear) {
  utl::activate_progress_tracker("third");
  utl::get_active_progress_tracker()->status("ASDF");

  utl::get_global_progress_trackers().clear();
  EXPECT_ANY_THROW(utl::get_active_progress_tracker());
}

TEST(progress_tracker_active_progress_tracker, use_after_clear) {
  auto tracker = utl::activate_progress_tracker("fourth");
  utl::get_global_progress_trackers().clear();

  auto const str = capture_cout([&] { tracker->status("detached tracker"); });

  EXPECT_TRUE(str.empty());
  EXPECT_TRUE(tracker->status_ == "detached tracker");
}
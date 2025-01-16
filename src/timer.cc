#include "utl/timer.h"

#include "utl/logging.h"

namespace utl {

scoped_timer::scoped_timer(std::string name)
    : name_{std::move(name)}, start_{std::chrono::steady_clock::now()} {
  utl::info("[{}] starting", name);
}

scoped_timer::~scoped_timer() {
  using namespace std::chrono;
  auto stop = steady_clock::now();
  double t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  utl::info("[{}] finished ({}ms)", name_, t);
}

void scoped_timer::print(std::string_view const message) const {
  using namespace std::chrono;
  auto const stop = steady_clock::now();
  double const t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  utl::info("[{}] {} ({}ms)", name_, message, t);
}

manual_timer::manual_timer(std::string name)
    : name_{std::move(name)}, start_{std::chrono::steady_clock::now()} {
  utl::info("[{}] starting", name_);
}

void manual_timer::stop_and_print() const {
  using namespace std::chrono;
  auto stop = steady_clock::now();
  double t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  utl::info("[{}] finished ({}ms)", name_, t);
}

void manual_timer::print(std::string_view const message) const {
  using namespace std::chrono;
  auto const stop = steady_clock::now();
  double const t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  utl::info("[{}] {} ({}ms)", name_, message, t);
}

}  // namespace utl
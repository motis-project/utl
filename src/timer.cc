#include "utl/timer.h"

#include "utl/logging.h"

namespace utl {

scoped_timer::scoped_timer(std::string name)
    : name_{std::move(name)}, start_{std::chrono::steady_clock::now()} {
  uLOG(info) << "[" << name_ << "] starting";
}

scoped_timer::~scoped_timer() {
  using namespace std::chrono;
  auto stop = steady_clock::now();
  double t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  uLOG(info) << "[" << name_ << "] finished"
             << " (" << t << "ms)";
}

void scoped_timer::print(std::string_view const message) const {
  using namespace std::chrono;
  auto const stop = steady_clock::now();
  double const t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  uLOG(info) << "[" << name_ << "] " << message << " (" << t << "ms)";
}

manual_timer::manual_timer(std::string name)
    : name_{std::move(name)}, start_{std::chrono::steady_clock::now()} {
  uLOG(info) << "[" << name_ << "] starting";
}

void manual_timer::stop_and_print() const {
  using namespace std::chrono;
  auto stop = steady_clock::now();
  double t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  uLOG(info) << "[" << name_ << "] finished"
             << " (" << t << "ms)";
}

void manual_timer::print(std::string_view const message) const {
  using namespace std::chrono;
  auto const stop = steady_clock::now();
  double const t =
      static_cast<double>(duration_cast<microseconds>(stop - start_).count()) /
      1000.0;
  uLOG(info) << "[" << name_ << "] " << message << " (" << t << "ms)";
}

}  // namespace utl
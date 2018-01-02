#pragma once

#include <utility>

namespace utl {

template <typename AccumulateFn, typename Acc>
struct accumulate_t {
  accumulate_t(AccumulateFn&& f, Acc&& acc)
      : fn_(std::forward<AccumulateFn>(f)), acc_(std::forward<Acc>(acc)) {}

  template <typename Range>
  friend Acc operator|(Range&& r, accumulate_t&& f) {
    auto it = r.begin();
    while (r.valid(it)) {
      f.acc_ = f.fn_(std::forward<Acc>(f.acc_), r.read(it));
      r.next(it);
    }
    return f.acc_;
  }

  AccumulateFn fn_;
  Acc acc_;
};

template <typename AccumulateFn, typename Acc>
accumulate_t<AccumulateFn, Acc> accumulate(AccumulateFn&& f, Acc&& acc) {
  return accumulate_t<AccumulateFn, Acc>(std::forward<AccumulateFn>(f),
                                         std::forward<Acc>(acc));
}

}  // namespace utl
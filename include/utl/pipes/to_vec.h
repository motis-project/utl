#include <vector>

namespace utl {

struct to_vec_t {
  template <typename Range>
  friend auto operator|(Range&& r, to_vec_t&&) {
    auto it = r.begin();
    using value_t = decltype(r.read(it));
    std::vector<value_t> v;
    while (r.valid(it)) {
      v.emplace_back(r.read(it));
      r.next(it);
    }
    return v;
  }
};

to_vec_t to_vec() { return to_vec_t(); }

}  // namespace utl
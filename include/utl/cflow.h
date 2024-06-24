#pragma once

namespace utl {

// Used like this:
// for_each(..., []() {
//    if (x) return utl::kContinue;
//    else   return utl::kBreak;
// });
enum class cflow { kContinue, kBreak };

}  // namespace utl
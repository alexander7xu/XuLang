#ifndef _SRC_UTILS_UTILS_HPP
#define _SRC_UTILS_UTILS_HPP

#include <memory>

namespace utils {

// https://stackoverflow.com/questions/51166178/recursive-macros-via-va-opt
#define _PARENS ()
#define _EXPAND(arg) _EXPAND1(_EXPAND1(_EXPAND1(_EXPAND1(arg))))
#define _EXPAND1(arg) arg
#define _FOR_EACH_HELPER(macro, x, ...) \
  macro(x) __VA_OPT__(_FOR_EACH_AGAIN _PARENS(macro, __VA_ARGS__))
#define _FOR_EACH_AGAIN() _FOR_EACH_HELPER
#define FOR_EACH(macro, ...) \
  __VA_OPT__(_EXPAND(_FOR_EACH_HELPER(macro, __VA_ARGS__)))

#define SRC_LOC \
  (__FILE__ + std::to_string(__LINE__) + " @ " + __PRETTY_FUNCTION__)

template <class T>
using Uptr = std::unique_ptr<T>;

template <class T>
using Sptr = std::shared_ptr<T>;

template <class DstT, class SrcT>
Sptr<DstT> CastS(const Sptr<SrcT> &sptr) {
  return std::dynamic_pointer_cast<DstT>(sptr);
}

}  // namespace utils

#endif  // _SRC_UTILS_UTILS_HPP

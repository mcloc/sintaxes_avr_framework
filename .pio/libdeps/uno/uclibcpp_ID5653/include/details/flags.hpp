#pragma once

#if defined(__clang__)
#  if __has_feature(cxx_rtti)
#    define __RTTI_ENABLED_ true
#  else
#    define __RTTI_ENABLED_ false
#  endif
#elif defined(__GNUG__) && defined(__GXX_RTTI)
#  define __RTTI_ENABLED_ true
#elif defined(_MSC_VER) && defined(_CPPRTTI)
#  define __RTTI_ENABLED_ true
#else
#  define __RTTI_ENABLED_ false
#endif

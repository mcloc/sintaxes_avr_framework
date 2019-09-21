#pragma once

#if defined(__has_cpp_attribute) && __has_cpp_attribute(deprecated)
#  define __ATTR_XSTR_(x) #x
#  define __ATTR_STR_(x) __ATTR_XSTR_(x)
#  define __DEPRECATED_(message) [[deprecated(__ATTR_STR_(message))]]
#elif defined(__GNUG__)
#  define __DEPRECATED_(message) __attribute__((__ATTR_STR_(message)))
#elif defined(_MSC_VER)
#  define __DEPRECATED_(message) __declspec(__ATTR_STR_(message))
#else
#  define __DEPRECATED_(message)
#endif

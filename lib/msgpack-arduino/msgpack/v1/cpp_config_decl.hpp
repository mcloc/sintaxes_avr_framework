//
// MessagePack for C++ C++03/C++11 Adaptation
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V1_CPP_CONFIG_DECL_HPP
#define MSGPACK_V1_CPP_CONFIG_DECL_HPP

#include "msgpack/versioning.hpp"

#if !defined(MSGPACK_USE_CPP03)
# if defined(_MSC_VER)
#  if _MSC_VER < 1900
#    define MSGPACK_USE_CPP03
#  endif
# elif (__cplusplus < 201103L)
#  define MSGPACK_USE_CPP03
# endif
#endif // MSGPACK_USE_CPP03


#if defined(MSGPACK_USE_CPP03)

#if defined(nullptr)
#  if defined (__cplusplus_cli)
#    define MSGPACK_NULLPTR __nullptr
#  else  // defined (__cplusplus_cli)
#    define MSGPACK_NULLPTR nullptr
#  endif // defined (__cplusplus_cli)
#else  // defined(nullptr)
#  define MSGPACK_NULLPTR (0)
#endif // defined(nullptr)

#include <memory>

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

template <typename T>
struct unique_ptr;

template <typename T>
T& move(T& t);

template <typename T>
T const& move(T const& t);

template <bool P, typename T = void>
struct enable_if;

template<typename T, T val>
struct integral_constant;

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

template<class T, class U>
struct is_same;

template<typename T>
struct underlying_type;

template<class T>
struct is_array;

template<class T>
struct remove_const;
template<class T>
struct remove_volatile;
template<class T>
struct remove_cv;

template<class T>
struct is_pointer;

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msgpack


#else  // MSGPACK_USE_CPP03

#if defined (__cplusplus_cli)
#  define MSGPACK_NULLPTR __nullptr
#else  // defined (__cplusplus_cli)
#  define MSGPACK_NULLPTR nullptr
#endif // defined (__cplusplus_cli)

#include <memory>
#include <boost_1_51_0.h>
#include <boost/tuple/tuple.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/functional/hash/hash.hpp>


namespace msgpack {
/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

    // unique_ptr
    using boost::interprocess::unique_ptr;
    // using std::make_unique; // since C++14
    using boost::hash;

    // utility
    using boost::move;
    using boost::swap;
    using boost::enable_if;
    using boost::is_same;
    using boost::underlying_type;
    using boost::is_array;
    using boost::remove_const;
    using boost::remove_volatile;
    using boost::remove_cv;
    using boost::is_pointer;

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond
}  // namespace msgpack


#endif // MSGPACK_USE_CPP03

#endif // MSGPACK_V1_CPP_CONFIG_DECL_HPP
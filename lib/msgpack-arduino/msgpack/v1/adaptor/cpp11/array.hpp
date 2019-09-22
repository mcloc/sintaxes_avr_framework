//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014-2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGPACK_V1_TYPE_CPP11_ARRAY_HPP
#define MSGPACK_V1_TYPE_CPP11_ARRAY_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/adaptor/adaptor_base.hpp"
#include "msgpack/adaptor/check_container_size.hpp"
#include "msgpack/meta.hpp"

#include <boost/array.hpp>

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

namespace detail {

namespace array {

template<typename T, std::size_t N1, std::size_t... I1, std::size_t N2, std::size_t... I2>
inline boost::array<T, N1+N2> concat(
    boost::array<T, N1>&& a1,
    boost::array<T, N2>&& a2,
    msgpack::seq<I1...>,
    msgpack::seq<I2...>) {
    return {{ boost::move(a1[I1])..., boost::move(a2[I2])... }};
}

template<typename T, std::size_t N1, std::size_t N2>
inline boost::array<T, N1+N2> concat(boost::array<T, N1>&& a1, boost::array<T, N2>&& a2) {
    return concat(boost::move(a1), boost::move(a2), msgpack::gen_seq<N1>(), msgpack::gen_seq<N2>());
}

template <typename T, std::size_t N>
struct as_impl {
    static boost::array<T, N> as(msgpack::object const& o) {
        msgpack::object* p = o.via.array.ptr + N - 1;
        return concat(as_impl<T, N-1>::as(o), boost::array<T, 1>{{p->as<T>()}});
    }
};

template <typename T>
struct as_impl<T, 1> {
    static boost::array<T, 1> as(msgpack::object const& o) {
        msgpack::object* p = o.via.array.ptr;
        return boost::array<T, 1>{{p->as<T>()}};
    }
};

template <typename T>
struct as_impl<T, 0> {
    static boost::array<T, 0> as(msgpack::object const&) {
        return boost::array<T, 0>();
    }
};

} // namespace array

} // namespace detail

template <typename T, std::size_t N>
struct as<boost::array<T, N>, typename boost::enable_if<msgpack::has_as<T>::value>::type> {
    boost::array<T, N> operator()(msgpack::object const& o) const {
        // if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
        // if(o.via.array.size > N) { throw msgpack::type_error(); }
        return detail::array::as_impl<T, N>::as(o);
    }
};

template <typename T, std::size_t N>
struct convert<boost::array<T, N>> {
    msgpack::object const& operator()(msgpack::object const& o, boost::array<T, N>& v) const {
        // if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
        // if(o.via.array.size > N) { throw msgpack::type_error(); }
        if(o.via.array.size > 0) {
            msgpack::object* p = o.via.array.ptr;
            msgpack::object* const pend = o.via.array.ptr + o.via.array.size;
            T* it = &v[0];
            do {
                p->convert(*it);
                ++p;
                ++it;
            } while(p < pend);
        }
        return o;
    }
};

template <typename T, std::size_t N>
struct pack<boost::array<T, N>> {
    template <typename Stream>
    msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const boost::array<T, N>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for(auto const& e : v) o.pack(e);
        return o;
    }
};

template <typename T, std::size_t N>
struct object_with_zone<boost::array<T, N>> {
    void operator()(msgpack::object::with_zone& o, const boost::array<T, N>& v) const {
        o.type = msgpack::type::define_array
        if(v.empty()) {
            o.via.array.ptr = MSGPACK_NULLPTR;
            o.via.array.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msgpack::object* p = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object)*size, MSGPACK_ZONE_ALIGNOF(msgpack::object)));
            o.via.array.size = size;
            o.via.array.ptr = p;
            for (auto const& e : v) *p++ = msgpack::object(e, o.zone);
        }
    }
};

} // namespace adaptor

/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msgpack

#endif // MSGPACK_V1_TYPE_CPP11_ARRAY_HPP

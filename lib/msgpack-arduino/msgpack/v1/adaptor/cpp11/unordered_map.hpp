//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014-2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V1_TYPE_CPP11_UNORDERED_MAP_HPP
#define MSGPACK_V1_TYPE_CPP11_UNORDERED_MAP_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/adaptor/adaptor_base.hpp"
#include "msgpack/adaptor/check_container_size.hpp"

#include <unordered_map.h>
#include <unordered_multimap.h>

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

using etl::unordered_map;
using etl::unordered_multimap;

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct as<
etl::unordered_map<K, V, Hash, Compare, Alloc>,
    typename boost::enable_if<msgpack::has_as<K>::value || msgpack::has_as<V>::value>::type> {
	etl::unordered_map<K, V, Hash, Compare, Alloc> operator()(msgpack::object const& o) const {
        // if (o.type != msgpack::type::MAP) { throw msgpack::type_error(); }
        msgpack::object_kv* p(o.via.map.ptr);
        msgpack::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        etl::unordered_map<K, V, Hash, Compare, Alloc> v;
        for (; p != pend; ++p) {
            v.emplace(p->key.as<K>(), p->val.as<V>());
        }
        return v;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct convert<etl::unordered_map<K, V, Hash, Compare, Alloc>> {
    msgpack::object const& operator()(msgpack::object const& o, etl::unordered_map<K, V, Hash, Compare, Alloc>& v) const {
        // if(o.type != msgpack::type::MAP) { throw msgpack::type_error(); }
        msgpack::object_kv* p(o.via.map.ptr);
        msgpack::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        etl::unordered_map<K, V, Hash, Compare, Alloc> tmp;
        for(; p != pend; ++p) {
            K key;
            p->key.convert(key);
            p->val.convert(tmp[boost::move(key)]);
        }
        v = boost::move(tmp);
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct pack<etl::unordered_map<K, V, Hash, Compare, Alloc>> {
    template <typename Stream>
        msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const etl::unordered_map<K, V, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for(typename etl::unordered_map<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<etl::unordered_map<K, V, Hash, Compare, Alloc>> {
    void operator()(msgpack::object::with_zone& o, const etl::unordered_map<K, V, Hash, Compare, Alloc>& v) const {
        o.type = msgpack::type::MAP;
        if(v.empty()) {
            o.via.map.ptr  = MSGPACK_NULLPTR;
            o.via.map.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msgpack::object_kv* p = static_cast<msgpack::object_kv*>(o.zone.allocate_align(sizeof(msgpack::object_kv)*size, MSGPACK_ZONE_ALIGNOF(msgpack::object_kv)));
            msgpack::object_kv* const pend = p + size;
            o.via.map.ptr  = p;
            o.via.map.size = size;
            typename etl::unordered_map<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin());
            do {
                p->key = msgpack::object(it->first, o.zone);
                p->val = msgpack::object(it->second, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};


template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct as<
    etl::unordered_multimap<K, V, Hash, Compare, Alloc>,
    typename boost::enable_if<msgpack::has_as<K>::value || msgpack::has_as<V>::value>::type> {
    etl::unordered_multimap<K, V, Hash, Compare, Alloc> operator()(msgpack::object const& o) const {
        // if (o.type != msgpack::type::MAP) { throw msgpack::type_error(); }
        msgpack::object_kv* p(o.via.map.ptr);
        msgpack::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        etl::unordered_multimap<K, V, Hash, Compare, Alloc> v;
        for (; p != pend; ++p) {
            v.emplace(p->key.as<K>(), p->val.as<V>());
        }
        return v;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct convert<etl::unordered_multimap<K, V, Hash, Compare, Alloc>> {
    msgpack::object const& operator()(msgpack::object const& o, etl::unordered_multimap<K, V, Hash, Compare, Alloc>& v) const {
        // if(o.type != msgpack::type::MAP) { throw msgpack::type_error(); }
        msgpack::object_kv* p(o.via.map.ptr);
        msgpack::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        etl::unordered_multimap<K, V, Hash, Compare, Alloc> tmp;
        for(; p != pend; ++p) {
            std::pair<K, V> value;
            p->key.convert(value.first);
            p->val.convert(value.second);
            tmp.insert(boost::move(value));
        }
        v = boost::move(tmp);
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct pack<etl::unordered_multimap<K, V, Hash, Compare, Alloc>> {
    template <typename Stream>
        msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const etl::unordered_multimap<K, V, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for(typename etl::unordered_multimap<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<etl::unordered_multimap<K, V, Hash, Compare, Alloc>> {
    void operator()(msgpack::object::with_zone& o, const etl::unordered_multimap<K, V, Hash, Compare, Alloc>& v) const {
        o.type = msgpack::type::MAP;
        if(v.empty()) {
            o.via.map.ptr  = MSGPACK_NULLPTR;
            o.via.map.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msgpack::object_kv* p = static_cast<msgpack::object_kv*>(o.zone.allocate_align(sizeof(msgpack::object_kv)*size, MSGPACK_ZONE_ALIGNOF(msgpack::object_kv)));
            msgpack::object_kv* const pend = p + size;
            o.via.map.ptr  = p;
            o.via.map.size = size;
            typename etl::unordered_multimap<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin());
            do {
                p->key = msgpack::object(it->first, o.zone);
                p->val = msgpack::object(it->second, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

} // namespace adaptor

/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msgpack


#endif // MSGPACK_V1_TYPE_CPP11_UNORDERED_MAP_HPP

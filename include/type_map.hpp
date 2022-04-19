// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Jonathan Woolf <jlwoolf@protonmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef MORE_MAPS_TYPE_MAP_HPP
#define MORE_MAPS_TYPE_MAP_HPP

#include <array>
#include <atomic>
#include <type_traits>
#include <utility>

#include <type_tools.hpp>

namespace more_maps
{
    GENERATE_HAS_MEMBER_FUNCTION(emplace, emplace)
    GENERATE_HAS_MEMBER_FUNCTION(find, find)
    GENERATE_HAS_MEMBER_FUNCTION(erase, erase)
    GENERATE_HAS_MEMBER_FUNCTION(insert, insert)
    GENERATE_HAS_MEMBER_FUNCTION(operator[], operator_index)

    template <
        class V,
        template <class...> class Map,
        class... MapArgs>
    class type_map
    {
    public:
        // type used to map values
        typedef Map<int, V, MapArgs...> map_type;
        // iterator of map
        typedef typename map_type::iterator iterator;
        // const_iterator of map
        typedef typename map_type::const_iterator const_iterator;

        // mapped type
        typedef V mapped_type;
        // type used by map to store entry pairs
        typedef typename map_type::value_type value_type;

        static_assert(has_insert_v<map_type, value_type>, "Map container must have insert function");
        static_assert(has_find_v<map_type, int &&>, "Map container must have find function");
        static_assert(has_erase_v<map_type, iterator>, "Map container must have erase function");
        static_assert(has_emplace_v<map_type, int &&>, "Map container must have emplace function");
        static_assert(has_operator_index_v<map_type, int &&>, "Map container must have operator[]");

        /**
         * @brief Returns an iterator to the first element of the map.
         *
         * @return const_iterator, Map<int,T,MapArgs...>::const_iterator
         */
        const_iterator begin() const { return _map.begin(); }
        /**
         * @brief Returns an iterator to the first element of the map.
         *
         * @return iterator, Map<int,T,MapArgs...>::iterator
         */
        iterator begin() { return _map.begin(); }

        /**
         * @brief Returns an iterator to the element following the last
         * element of the map.
         *
         * @return const_iterator, Map<int,T,MapArgs...>::const_iterator
         */
        const_iterator end() const { return _map.end(); }
        /**
         * @brief Returns an iterator to the element following the last
         * element of the map.
         *
         * @return iterator, Map<int, T, MapArgs...>::iterator
         */
        iterator end() { return _map.end(); }

        /**
         * @brief Finds an element with a key equivalent to the passed
         * type.
         *
         * @tparam K type used as key
         * @return const_iterator, Map<int,T,MapArgs...>::const_iterator
         */
        template <class T>
        is_detected_t<find_t, map_type, const int> find() const { return _map.find(getTypeId<T>()); }
        /**
         * @brief Finds an element with a key equivalent to the passed
         * type.
         *
         * @tparam K type used as key
         * @return const_iterator, Map<int,T,MapArgs...>::const_iterator
         */
        template <class T>
        is_detected_t<find_t, map_type, int> find() { return _map.find(getTypeId<T>()); }

        template <class T>
        bool contains() { return find<T>() != end(); }

        template <class T, class... Args>
        is_detected_t<emplace_t, map_type, Args &&...> emplace(Args &&...args)
        {
            return _map.emplace(getTypeId<T>(), args...);
        }

        template <class T>
        is_detected_t<insert_t, map_type, const value_type &> insert(const V &value)
        {
            return _map.insert(static_cast<const value_type &>(value_type{getTypeId<T>(), value}));
        }

        template <class T>
        is_detected_t<insert_t, map_type, value_type&&> insert(V &&value)
        {
            return _map.insert(value_type{getTypeId<T>(), std::forward<V>(value)});
        }

        template <class... TArgs, typename = typename std::enable_if_t<(sizeof...(TArgs) > 1)>>
        std::array<is_detected_t<insert_t, map_type, const value_type &>, sizeof...(TArgs)> insert(const V &value)
        {
            return {emplace<TArgs>(std::forward<const V &>(value))...};
        }

        template <class... TArgs, class... VArgs, typename = typename std::enable_if_t<sizeof...(TArgs) == sizeof...(VArgs) && std::conjunction_v<std::is_same<V, VArgs>...>>>
        std::array<is_detected_t<insert_t, map_type, value_type &&>, sizeof...(TArgs)> insert(VArgs &&...values)
        {
            return {emplace<TArgs>(std::forward<V>(values))...};
        }

        is_detected_t<erase_t, map_type, iterator> erase(iterator pos) {
            return _map.erase(pos);
        }

        is_detected_t<erase_t, map_type, const_iterator> erase(const_iterator pos)
        {
            return _map.erase(pos);
        }

        template<typename T>
        is_detected_t<erase_t, map_type, int> erase() {
            return _map.erase(getTypeId<T>());
        }

        template <typename... TArgs, typename = typename std::enable_if_t<(sizeof...(TArgs) > 1)>>
        is_detected_t<erase_t, map_type, int> erase()
        {
            int sum = 0;
            for(size_t v : {_map.erase(getTypeId<TArgs>())...}) sum+= v;
            return sum;
        }

        /**
         * @brief Returns a reference to the mapped value of the element
         * for a specified type.
         *
         * @tparam K type used as key
         * @return T&, reference to mapped value
         */
        template <class T>
        V &get() { return _map[getTypeId<T>()]; }

        /**
         * @brief Returns a reference to the mapped value of the element
         * for a specified type.
         *
         * @tparam K type used as key
         * @return const T&, reference to mapped value
         */
        template <class T>
        const V &get() const { return static_cast<const V&>(_map[getTypeId<T>()]); }

    private:
        template <class T>
        inline static int getTypeId()
        {
            static const int id = _typeId++;
            return id;
        }

        static std::atomic_int _typeId;
        map_type _map;
    };

    template <class V,
              template <class...> class Map,
              class... MapArgs>
    std::atomic_int type_map<V, Map, MapArgs...>::_typeId(0);
}

#endif // MORE_MAPS_TYPE_MAP_HPP
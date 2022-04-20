#ifndef MORE_MAPS_CAST_MAP_HPP
#define MORE_MAPS_CAST_MAP_HPP

namespace more_maps
{
#ifndef HAS_AT
#define HAS_AT
    GENERATE_HAS_MEMBER_FUNCTION(at, at)
#endif

#ifndef HAS_EMPLACE
#define HAS_EMPLACE
    GENERATE_HAS_MEMBER_FUNCTION(emplace, emplace)
#endif

#ifndef HAS_ERASE
#define HAS_ERASE
    GENERATE_HAS_MEMBER_FUNCTION(erase, erase)
#endif

#ifndef HAS_FIND
#define HAS_FIND
    GENERATE_HAS_MEMBER_FUNCTION(find, find)
#endif

#ifndef HAS_INSERT
#define HAS_INSERT
    GENERATE_HAS_MEMBER_FUNCTION(insert, insert)
#endif

#ifndef HAS_OPERATOR_INDEX
#define HAS_OPERATOR_INDEX
    GENERATE_HAS_MEMBER_FUNCTION(operator[], operator_index)
#endif

    template <
        class K,
        class V,
        template <class...> class Map,
        class... MapArgs>
    class cast_map
    {
    public:
        // type used to map values
        typedef Map<K, V, MapArgs...> map_type;
        // iterator of map
        typedef typename map_type::iterator iterator;
        // const_iterator of map
        typedef typename map_type::const_iterator const_iterator;

        // key type
        typedef K key_type;
        // mapped type
        typedef V mapped_type;
        // type used by map to store entry pairs
        typedef typename map_type::value_type value_type;

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
        is_detected_t<find_t, map_type, const key_type &> find(const K &key) const
        {
            static_assert(has_find_v<map_type, key_type>, "Map container must have find function");
            return _map.find(key);
        }
        /**
         * @brief Finds an element with a key equivalent to the passed
         * type.
         *
         * @tparam K type used as key
         * @return const_iterator, Map<int,T,MapArgs...>::const_iterator
         */
        is_detected_t<find_t, map_type, key_type &&> find(K &&key)
        {
            static_assert(has_find_v<map_type, key_type>, "Map container must have find function");
            return _map.find(key);
        }

        bool contains(const K &key) { return find(key) != end(); }
        bool contains(K &&key) { return find(key) != end(); }

        /**
         * @brief
         *
         * @tparam Args
         * @param key
         * @param args
         * @return is_detected_t<emplace_t, map_type, Args &&...>
         */
        template <class... Args>
        is_detected_t<emplace_t, map_type, Args &&...> emplace(K &&key, Args &&...args)
        {
            static_assert(has_emplace_v<map_type, key_type, Args...>, "more_maps::cast_map container must have emplace function with paramter type key_type & Args...");
            return _map.emplace(key, args...);
        }

        /**
         * @brief
         *
         * @param key
         * @param value
         * @return is_detected_t<insert_t, map_type, const value_type &>
         */
        is_detected_t<insert_t, map_type, const value_type &> insert(const K &key, const V &value)
        {
            static_assert(has_insert_v<map_type, value_type>, "more_maps::cast_map container must have insert function with parameter type value_type");
            return _map.insert(static_cast<const value_type &>(value_type{key, value}));
        }
        template <class T, typename = typename std::enable_if_t<std::is_convertible_v<T, V>>>
        is_detected_t<insert_t, map_type, const value_type &> insert(const K &key, const T &value)
        {
            static_assert(has_insert_v<map_type, value_type>, "more_maps::cast_map container must have insert function with parameter type value_type");
            return _map.insert(static_cast<const value_type &>(value_type{key, static_cast<const T &>(value)}));
        }
        is_detected_t<insert_t, map_type, value_type &&> insert(K &&key, V &&value)
        {
            static_assert(has_insert_v<map_type, value_type>, "more_maps::cast_map container must have insert function with parameter type value_type");
            return _map.insert(value_type{std::forward<K>(key), std::forward<V>(value)});
        }
        template <class T, typename = typename std::enable_if_t<std::is_convertible_v<T, V>>>
        is_detected_t<insert_t, map_type, value_type &&> insert(K &&key, T &&value)
        {
            static_assert(has_insert_v<map_type, value_type>, "more_maps::cast_map container must have insert function with parameter type value_type");
            return _map.insert(value_type{std::forward<K>(key), std::forward<V>(value)});
        }

        /**
         * @brief
         *
         * @param pos
         * @return is_detected_t<erase_t, map_type, iterator>
         */
        is_detected_t<erase_t, map_type, iterator> erase(iterator pos)
        {
            static_assert(has_erase_v<map_type, iterator>, "more_maps::cast_map map container must have erase function with parameter type iterator");
            return _map.erase(pos);
        }
        is_detected_t<erase_t, map_type, const_iterator> erase(const_iterator pos)
        {
            static_assert(has_erase_v<map_type, const_iterator>, "more_maps::cast_map map container must have erase function with parameter type const_iterator");
            return _map.erase(pos);
        }

        /**
         * @brief
         *
         * @param key
         * @return is_detected_t<erase_t, map_type, key_type>
         */
        is_detected_t<erase_t, map_type, key_type> erase(const K &key)
        {
            static_assert(has_erase_v<map_type, key_type>, "more_maps::cast_map map container must have erase function with parameter type key_type");
            return _map.erase(key);
        }
        is_detected_t<erase_t, map_type, const key_type &> erase(K &&key)
        {
            static_assert(has_erase_v<map_type, key_type>, "more_maps::cast_map map container must have erase function with parameter type key_type");
            return _map.erase(key);
        }

        /**
         * @brief 
         * 
         * @param key 
         * @return is_detected_t<at_t, map_type, const key_type &> 
         */
        is_detected_t<at_t, map_type, const key_type &> at(const K& key) {
            static_assert(has_at_v<map_type, key_type>, "more_maps::cast_map container must have at function with parameter type key_type");
            return _map.at(key);
        }

        is_detected_t<at_t, map_type, const key_type &> at(const K &key) const
        {
            static_assert(has_at_v<map_type, key_type>, "more_maps::cast_map container must have at function with parameter type key_type");
            return _map.at(key);
        }

        /**
         * @brief 
         * 
         * @tparam T 
         * @param key 
         * @return T 
         */
        template<typename T>
        T at(const K &key)
        {
            static_assert(has_at_v<map_type, key_type>, "more_maps::cast_map container must have at function with parameter type key_type");
            return static_cast<T>(_map.at(key));
        }

        /**
         * @brief 
         * 
         * @param key 
         * @return is_detected_t<operator_index_t, map_type, key_type> 
         */
        is_detected_t<operator_index_t, map_type, key_type> operator[](const K&key) {
            static_assert(has_operator_index_v<map_type, key_type>, "more_maps::cast_map container must have operator[] with parameter type key_type");
            return _map[key];
        }

        is_detected_t<operator_index_t, map_type, key_type> operator[](K &&key)
        {
            static_assert(has_operator_index_v<map_type, key_type>, "more_maps::cast_map container must have operator[] with parameter type key_type");
            return _map[key];
        }

        /**
         * @brief 
         * 
         * @tparam T 
         * @param key 
         * @return T 
         */
        template<typename T>
        T get(const K&key) {
            static_assert(has_operator_index_v<map_type, key_type>, "more_maps::cast_map container must have operator[] with parameter type key_type");
            return static_cast<T>(_map[key]);
        }
        template <typename T>
        T get(K &&key)
        {
            static_assert(has_operator_index_v<map_type, key_type>, "more_maps::cast_map container must have operator[] with parameter type key_type");
            return static_cast<T>(_map[key]);
        }

    private:
        map_type _map;
    };

} // namespace more_maps

#endif // MORE_MAPS_CAST_MAP_HPP
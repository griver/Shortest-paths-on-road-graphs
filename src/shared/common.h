#pragma once

//#define _HAS_ITERATOR_DEBUGGING 0

#include <cstdlib>

#include <list>
using std::list;

#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <sstream>
using std::stringstream;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <queue>
using std::priority_queue;
using std::queue;

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <boost/optional.hpp>
using boost::optional;

#include <boost/scoped_ptr.hpp>
using boost::scoped_ptr;
#include <boost/scoped_array.hpp>
using boost::scoped_array;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <cmath>

#pragma warning (push)
#pragma warning (disable : 4244)
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/vector.hpp>
#pragma warning (pop)

/*#if defined (NDEBUG) || !defined (_MSC_VER)
    #define USE_BOOST_HASHMAP
#else
    #define USE_TR1_HASHMAP
#endif*/

#define USE_BOOST_HASHMAP



#if defined ( USE_TR1_HASHMAP )
    #include <unordered_map>
    using std::tr1::unordered_map;
    #include <unordered_set>
    using std::tr1::unordered_set;
#elif defined ( USE_BOOST_HASHMAP )
    #include <boost/unordered_map.hpp>
    using boost::unordered_map;
    #include <boost/unordered_set.hpp>
    using boost::unordered_set;
#else
    #error No hash map is selected: USE_TR1_HASHMAP or USE_BOOST_HASHMAP should be defined!
#endif


template <typename K, typename V>
inline const V& unordered_safe_find_const(const unordered_map<K,V> &map, const K &key)
{
    /*unordered_map<K,V>::const_iterator it = map.find(key);
    assert(it != map.end());
    return it->second;*/
    return map.at(key);
}
template <typename K, typename V>
inline V& unordered_safe_find(unordered_map<K,V> &map, const K &key)
{
    /*unordered_map<K,V>::iterator it = map.find(key);
    assert(it != map.end());
    return it->second;*/
    return map.at(key);
}

template <typename K, typename V>
inline bool& unordered_exists (unordered_map<K,V> &map, const K &key)
{
    return (map.count(key) != 0);
}

template <typename K, typename V>
inline bool& unordered_exists (unordered_set<K,V> &set, const K &key)
{
    return (set.count(key) != 0);
}

#undef min
#undef max
#include <limits>
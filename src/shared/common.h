#pragma once

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

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <boost/optional.hpp>

#include <boost/scoped_ptr.hpp>
using boost::scoped_ptr;
#include <boost/scoped_array.hpp>
using boost::scoped_array;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#if defined (NDEBUG) || !defined (_WIN32)
    #define USE_BOOST_HASHMAP
#else
    #define USE_TR1_HASHMAP
#endif


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
    unordered_map<K,V>::const_iterator it = map.find(key);
    assert(it != map.end());
    return it->second;
}
template <typename K, typename V>
inline V& unordered_safe_find(unordered_map<K,V> &map, const K &key)
{
    unordered_map<K,V>::iterator it = map.find(key);
    assert(it != map.end());
    return it->second;
}



#undef min
#undef max
#include <limits>
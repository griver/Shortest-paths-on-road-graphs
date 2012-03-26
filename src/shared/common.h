#pragma once

#include <iostream>
using std::cout;
using std::endl;

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

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#if defined (NDEBUG)
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
	#include <boost/unordered_map>
	using boost::unordered_map;
	#include <boost/unordered_set>
	using boost::unordered_set;
#else
	#error No hash map is selected: USE_TR1_HASHMAP or USE_BOOST_HASHMAP should be defined!
#endif
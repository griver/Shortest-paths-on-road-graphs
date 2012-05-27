// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//


#pragma once


#ifndef __int64
typedef long long __int64;
#endif

#if defined (WIN32)
#include <Windows.h>
#else
#include "../shared/linux_time.h"
#endif

#include "../shared/common.h"
#include <boost/algorithm/string.hpp>
#include "../rapidxml/rapidxml.hpp"

#if defined USE_OPENMP
#include <omp.h>
#endif

#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"





/*//--old windows-----------

#pragma once

#include <Windows.h>
#include "../shared/common.h"
#include <boost/algorithm/string.hpp>
#include "../rapidxml/rapidxml.hpp"

//#include "../shared/graph.h"
#include <omp.h>
//*/

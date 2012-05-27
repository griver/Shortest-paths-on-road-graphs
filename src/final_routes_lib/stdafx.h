
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
#include "../rapidxml/rapidxml.hpp"

#if defined USE_OPENMP
#include <omp.h>
#endif

#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"

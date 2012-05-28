
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
//---tnr---
//#include "../tnr_utils/tnr_utils.h"
#include "../transit_node/tnr_path_finder.h"
#include "../tnr_utils/tnr_loader.h"
#include "../tnr_utils/grid_info.h"
//--/tnr---

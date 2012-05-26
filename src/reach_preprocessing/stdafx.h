// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define DISABLE_VIS

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

#include "reach_common.h"


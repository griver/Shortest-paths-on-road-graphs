#pragma once

#include "../shared/grid.h"

class raster
{
public:
    inline raster(const vis_coord &mins, const vis_coord &maxs, size_t width, size_t height);

private:
    grid gr;
};

raster::raster(const vis_coord &mins, const vis_coord &maxs, size_t width, size_t height)
    : gr (mins, maxs, width, height)
{

}

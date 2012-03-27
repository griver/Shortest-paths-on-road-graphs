#pragma once

class draw_scope
{
public:
    virtual coord<int>      world2screen (coord<float>  )   const = 0;
    virtual coord<float>    screen2world (coord<int>    )   const = 0;

    virtual void drag (coord<int>, coord<int>) = 0;
    virtual void zoom (float, coord<int>) = 0;
};
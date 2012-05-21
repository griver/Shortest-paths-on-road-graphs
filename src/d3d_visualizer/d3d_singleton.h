#pragma once

class d3d_singleton
{
public:
    ~d3d_singleton();
    LPDIRECT3D9 get ();
    static shared_ptr<d3d_singleton> get_inst();
private:
    d3d_singleton ();
    LPDIRECT3D9 p_;
};


#include "stdafx.h"
#include "d3d_singleton.h"

d3d_singleton::d3d_singleton()
{
    p_ = Direct3DCreate9( D3D_SDK_VERSION );
    assert (p_ != NULL);
}

d3d_singleton::~d3d_singleton()
{
    p_->Release();
}

shared_ptr<d3d_singleton> d3d_singleton::get_inst()
{
    static shared_ptr<d3d_singleton> ptr (new d3d_singleton ());
    return ptr;
}

LPDIRECT3D9 d3d_singleton::get()
{
    return p_;
}

#pragma once

#include "../shared/visualizer.h"
#include "../shared/draw_scope.h"
#include "../shared/client.h"

#include "d3d_singleton.h"



class d3d_vis : public draw_scope, public visualizer
{
public:
    d3d_vis (int width, int height);
    virtual ~d3d_vis();

    vb_id   create_vb   (size_t);
    ib_id   create_ib   (size_t);
    void    free_vb     (vb_id);
    void    free_ib     (ib_id);
    
    void draw_buffers     (vb_id verts, size_t n_verts, ib_id inds, size_t n_inds);
    void draw_text      (coord<int>, const std::string&);

    void draw_begin ();
    void draw_end   ();

    void set_color  (unsigned char r, unsigned char g, unsigned char b);
    void unset_color ();
    void set_bg_color  (unsigned char r, unsigned char g, unsigned char b);

    void draw_rect  (coord<int>, coord<int>);
    void draw_line  (coord<int>, coord<int>);

    //void build_graph (vis_graph& g, vb_id, ib_id);

    b_vertex* lock_vb   (vb_id, size_t start, size_t size, unsigned int flags = 0);
    void unlock_vb      (vb_id);

    b_edge* lock_ib     (ib_id, size_t start, size_t size, unsigned int flags = 0);
    void unlock_ib      (ib_id);

    void resize (int width, int height);

    void reset_scope(coord<float> org, float scale);
    coord<int>          world2screen (coord<float>  )   const;
    coord<float>        screen2world (coord<int>    )   const;
    void drag (coord<int>);
    void zoom (float, coord<int>);
    void set_scale (float scale);
    void update_matrices ();

    void set_client (client* pcl);

private:
    void d3d_init ();
    void d3d_shutdown ();

private:
    void in_world ();
    void in_screen ();

    void update_projection ();

    void static safe_release (IUnknown* p);
    void static check_succeded (HRESULT hr);

    void release_mini_resources ();
    void set_mini_resources ();

private:
    shared_ptr<d3d_singleton> pd3d_;
    D3DPRESENT_PARAMETERS d3dpp_;

    HWND                hwnd_;
    LPDIRECT3DDEVICE9   pdevice_;
    LPD3DXFONT          pfont_;
    LPDIRECT3DVERTEXDECLARATION9 pvdeclsingle_, pvdeclmulti_;
    LPDIRECT3DVERTEXBUFFER9 psinglecolor_;
    LPDIRECT3DVERTEXBUFFER9 prect_;

    D3DCOLOR color_;
    D3DCOLOR bg_color_;
    
    D3DXVECTOR3 ofs_;
    float scale_;
    float rot_;

    std::vector<IDirect3DVertexBuffer9*> vbs_;
    std::vector<IDirect3DIndexBuffer9*> ibs_;
    D3DXMATRIX mw2s, ms2w;
};

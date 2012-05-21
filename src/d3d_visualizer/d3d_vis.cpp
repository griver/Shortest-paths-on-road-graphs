#include "stdafx.h"
#include "d3d_vis.h"
#include "d3d_singleton.h"


HWND create_window_from_console (int width, int height, WNDPROC WndProc);
LRESULT CALLBACK d3d_vis_wndproc ( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );


extern client* g_pClient;

d3d_vis::d3d_vis (int width, int height)
    : pd3d_ (d3d_singleton::get_inst())
    , color_ (D3DCOLOR_XRGB(0,0,0))
    , bg_color_ (D3DCOLOR_XRGB(255,255,255))
    , ofs_ (0, 0, 0)
    , scale_ (1.0f)
    , rot_ (-3.14159f * 0.5f)

    , pdevice_      (NULL)
    , pfont_        (NULL)
    , pvdeclsingle_ (NULL)
    , pvdeclmulti_  (NULL)
    , psinglecolor_ (NULL)
    , prect_        (NULL)

{

    hwnd_ = create_window_from_console(width, height, d3d_vis_wndproc);
    d3d_init ();

    check_succeded (D3DXCreateFont( pdevice_, 16, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pfont_ ));
    assert (pfont_ != NULL);

    update_projection();
    update_matrices();

    set_mini_resources();

    check_succeded (pdevice_->CreateVertexBuffer(sizeof (DWORD), 0, 0, D3DPOOL_MANAGED, &psinglecolor_, NULL));
    check_succeded (pdevice_->CreateVertexBuffer(sizeof (b_vertex) * 5, 0, 0, D3DPOOL_MANAGED, &prect_, NULL));

    check_succeded (pdevice_->SetStreamSource(1, psinglecolor_, 0, 0));

    set_color (255, 255, 255);
    in_screen();
}

d3d_vis::~d3d_vis()
{
    release_mini_resources();

    safe_release(prect_);
    safe_release(psinglecolor_);
    safe_release(pfont_);
    safe_release(pdevice_);
}

void d3d_vis::d3d_init()
{
    D3DDISPLAYMODE d3ddm;
    pd3d_->get()->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

    // Set up the structure used to create the D3DDevice
    ZeroMemory(&d3dpp_, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp_.Windowed = TRUE;
    d3dpp_.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp_.BackBufferFormat = d3ddm.Format;
    d3dpp_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the D3DDevice
    check_succeded(pd3d_->get()->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd_,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp_, &pdevice_ ));
    assert (pdevice_ != NULL);

    // Turn off culling, so we see the front and back of the triangle
    check_succeded (pdevice_->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));

    // Turn off D3D lighting, since we are providing our own vertex colors
    check_succeded (pdevice_->SetRenderState( D3DRS_LIGHTING, FALSE ));

    //check_succeded (g_pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_DIFFUSE ));
}

vb_id d3d_vis::create_vb( size_t size )
{
    IDirect3DVertexBuffer9 *res;

    check_succeded(pdevice_->CreateVertexBuffer(  sizeof(b_vertex)*size, 
        0,
        b_vertex::fvf,
        D3DPOOL_MANAGED,
        &res,
        NULL));

    vbs_.push_back(res);
    return vbs_.size() - 1;
}

ib_id d3d_vis::create_ib( size_t size )
{
    IDirect3DIndexBuffer9 *res;

    check_succeded(pdevice_->CreateIndexBuffer(   sizeof(b_edge)*size,
        0,
        D3DFMT_INDEX32,
        D3DPOOL_MANAGED,
        &res,
        NULL));


    ibs_.push_back(res);
    return ibs_.size() - 1;
}

void d3d_vis::free_vb( vb_id i )
{
    safe_release(vbs_[i]);
}

void d3d_vis::free_ib( ib_id i )
{
    safe_release(ibs_[i]);
}

void d3d_vis::draw_text( coord<int> c, const std::string& str)
{
    RECT myrect;
    GetClientRect (hwnd_, &myrect);
    myrect.left = c.x;
    myrect.top = c.y;
    pfont_->DrawTextA(NULL, str.c_str(), str.length(), &myrect,  0, color_);
}

void d3d_vis::draw_begin()
{
    check_succeded(pdevice_->Clear( 0, NULL, D3DCLEAR_TARGET, 
        bg_color_, 1.0f, 0 ));
    

    /*static bool shader_printed = false;
    if (!shader_printed)
    {
        IDirect3DVertexShader9 *pshd = NULL;
        pdevice_->GetVertexShader(&pshd);

        stringstream ss;
        ss << "Vertex shader: 0x" << pshd << endl;
        cout << ss.str();

        shader_printed = true;
    }*/
    check_succeded (pdevice_->BeginScene());
}

void d3d_vis::draw_end()
{
    check_succeded (pdevice_->EndScene());
    check_succeded (pdevice_->Present( NULL, NULL, NULL, NULL ));
}

void d3d_vis::set_color(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int *ptr;
    check_succeded (psinglecolor_->Lock(0, sizeof (unsigned int), reinterpret_cast<void**>(&ptr), 0));
    color_ = *ptr = D3DCOLOR_XRGB(r,g,b);
    check_succeded (psinglecolor_->Unlock ());
    check_succeded (pdevice_->SetVertexDeclaration(pvdeclsingle_));
    //check_succeded (pdevice_->SetStreamSource(1, psinglecolor_, 0, 0));
}


void d3d_vis::unset_color()
{
    check_succeded (pdevice_->SetVertexDeclaration(pvdeclmulti_));
}

void d3d_vis::set_bg_color(unsigned char r, unsigned char g, unsigned char b)
{
    bg_color_ = D3DCOLOR_XRGB(r,g,b);
}

void d3d_vis::draw_rect( coord<int> ui, coord<int> vi )
{
    coord<float> u = ui;
    coord<float> v = vi;

    b_vertex vertices[] =
    {
        { u.x,    u.y,     0.0f,    0xFFFFFFFF},
        { v.x,    u.y,     0.0f,    0xFFFFFFFF},
        { v.x,    v.y,     0.0f,    0xFFFFFFFF},
        { u.x,    v.y,     0.0f,    0xFFFFFFFF},
        { u.x,    u.y,     0.0f,    0xFFFFFFFF},
    };

    b_vertex *ptr;
    check_succeded (prect_->Lock(0, sizeof (b_vertex) * 5, reinterpret_cast<void**>(&ptr), 0));
    memcpy (ptr, vertices, sizeof (b_vertex) * 5);
    check_succeded (prect_->Unlock());

    check_succeded (pdevice_->SetStreamSource(0, prect_, 0, sizeof (b_vertex)));
    check_succeded (pdevice_->DrawPrimitive (D3DPT_LINESTRIP, 0, 4));
}

void d3d_vis::draw_rect_world(coord<float> u, coord<float> v)
{
    b_vertex vertices[] =
    {
        { u.x,    u.y,     0.0f,    0xFFFFFFFF},
        { v.x,    u.y,     0.0f,    0xFFFFFFFF},
        { v.x,    v.y,     0.0f,    0xFFFFFFFF},
        { u.x,    v.y,     0.0f,    0xFFFFFFFF},
        { u.x,    u.y,     0.0f,    0xFFFFFFFF},
    };

    b_vertex *ptr;
    check_succeded (prect_->Lock(0, sizeof (b_vertex) * 5, reinterpret_cast<void**>(&ptr), 0));
    memcpy (ptr, vertices, sizeof (b_vertex) * 5);
    check_succeded (prect_->Unlock());

    in_world();
    check_succeded (pdevice_->SetStreamSource(0, prect_, 0, sizeof (b_vertex)));
    check_succeded (pdevice_->DrawPrimitive (D3DPT_LINESTRIP, 0, 4));
    in_screen();
}

void d3d_vis::draw_line( coord<int> ui, coord<int> vi )
{
    coord<float> u = ui;
    coord<float> v = vi;

    b_vertex vertices[] =
    {
        { u.x,    u.y,     0.0f,  0xFFFFFFFF},
        { v.x,    v.y,     0.0f,  0xFFFFFFFF},
    };

    b_vertex *ptr;
    check_succeded (prect_->Lock(0, sizeof (b_vertex) * 2, reinterpret_cast<void**>(&ptr), 0));
    memcpy (ptr, vertices, sizeof (b_vertex) * 2);
    check_succeded (prect_->Unlock());

    check_succeded (pdevice_->SetStreamSource(0, prect_, 0, sizeof (b_vertex)));
    check_succeded (pdevice_->DrawPrimitive (D3DPT_LINESTRIP, 0, 1));
}


void d3d_vis::in_world()
{
    /*D3DXMATRIX world, sc, ofs;

    D3DXMatrixScaling       (&sc, scale_, scale_, scale_);
    D3DXMatrixTranslation   (&ofs, ofs_.x, ofs_.y, 0);
    world = sc*ofs;*/
    check_succeded (pdevice_->SetTransform (D3DTS_WORLD, &mw2s));
}


void d3d_vis::in_screen()
{
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    check_succeded (pdevice_->SetTransform (D3DTS_WORLD, &world));
}


void d3d_vis::draw_buffers( vb_id verts, size_t n_verts, ib_id inds, size_t n_inds )
{
    in_world();

    check_succeded (pdevice_->SetStreamSource(0, vbs_[verts], 0, sizeof (b_vertex)));
    check_succeded (pdevice_->SetIndices(ibs_[inds]));
    check_succeded (pdevice_->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, n_verts, 0, n_inds));

    in_screen();
}
void d3d_vis::draw_buffers(vb_id verts, size_t verts_offset, size_t n_verts, ib_id inds, size_t inds_offset, size_t n_inds)
{
    in_world();

    check_succeded (pdevice_->SetStreamSource(0, vbs_[verts], verts_offset * sizeof(b_vertex), sizeof (b_vertex)));
    check_succeded (pdevice_->SetIndices(ibs_[inds]));
    check_succeded (pdevice_->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, n_verts, inds_offset * 2, n_inds));

    in_screen();
}

void d3d_vis::update_matrices()
{
    D3DXMATRIX sc, ofs, rot;

    D3DXMatrixScaling       (&sc, scale_, scale_, scale_);
    D3DXMatrixTranslation   (&ofs, ofs_.x, ofs_.y, ofs_.z);
    D3DXMatrixRotationZ     (&rot, rot_);
    mw2s = sc*ofs*rot;
    
    D3DXMatrixRotationZ     (&rot, -rot_);
    D3DXMatrixTranslation   (&ofs, -ofs_.x, -ofs_.y, -ofs_.z);
    D3DXMatrixScaling       (&sc, 1.0f/scale_, 1.0f/scale_, 1.0f/scale_);
    ms2w = rot*ofs*sc;
}

void d3d_vis::reset_scope(coord<float> org, float scale)
{
    scale_ = scale;
    ofs_ = D3DXVECTOR3(org.x, org.y, 0) * (-scale);
    update_matrices();
}

coord<int> d3d_vis::world2screen( coord<float> world ) const
{
    D3DXVECTOR3 vec_world (world.x, world.y, 0), vec_screen;
    D3DXVec3TransformCoord(&vec_screen, &vec_world, &mw2s);
    //vec_world = mw2s * vec_world;
    /*world *= scale_;
    world += ofs_;*/
    coord<int> screen (static_cast<int>(vec_screen.x), static_cast<int>(vec_screen.y));
    return screen;
}

coord<float> d3d_vis::screen2world( coord<int> screen ) const
{
    D3DXVECTOR3 vec_world, vec_screen (screen.x, screen.y, 0);
    D3DXVec3TransformCoord(&vec_world, &vec_screen, &ms2w);
    /*coord<float> world (static_cast<float>(screen.x), static_cast<float>(screen.y));
    world -= ofs_;
    world *= 1.0f/scale_;*/
    coord<int> world (static_cast<int>(vec_world.x), static_cast<int>(vec_world.y));
    return world;
}

void d3d_vis::drag(coord<int> d)
{
/*
    coord<float> world_ofs (static_cast<float>(screen1.x-screen2.x), static_cast<float>(screen1.y-screen2.y));

    ofs_.x += world_ofs.x;
    ofs_.y += world_ofs.y;
*/
    D3DXVECTOR3 src(d.x, d.y, 0), dst;
    D3DXMATRIX m;
    D3DXMatrixRotationZ(&m, -rot_);
    D3DXVec3TransformCoord(&dst, &src, &m);

    ofs_ += dst;
    update_matrices();
}

void d3d_vis::zoom(float z, coord<int> s)
{
    D3DXVECTOR3 src (s.x, s.y, 0), dst;
    D3DXMATRIX m;
    D3DXMatrixRotationZ(&m, -rot_);
    D3DXVec3TransformCoord(&dst, &src, &m);
    ofs_ = (1.0f-z) * dst + z * ofs_;
    scale_*=z;
    
    update_matrices();
}

/*

void d3d_vis::set_scale( float scale )
{
    scale_ = scale;
}*/

b_vertex* d3d_vis::lock_vb( vb_id pvb, size_t start, size_t size, unsigned int flags )
{
    b_vertex* pv;
    HRESULT hr = vbs_[pvb]->Lock(start, size*sizeof(b_vertex), (void**)&pv, 0);
    
    if (FAILED (hr))
        return NULL;

    return pv;
}

void d3d_vis::unlock_vb( vb_id pvb )
{
    check_succeded (vbs_[pvb]->Unlock());
}

b_edge* d3d_vis::lock_ib( ib_id peb, size_t start, size_t size, unsigned int flags )
{
    b_edge* pe;
    HRESULT hr = ibs_[peb]->Lock(start, size*sizeof(b_edge), (void**)&pe, 0);

    if (FAILED (hr))
        return NULL;

    return pe;
}

void d3d_vis::unlock_ib( ib_id peb )
{
    check_succeded (ibs_[peb]->Unlock());
}

void d3d_vis::resize(int width, int height)
{
    
    /*D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;*/

    d3dpp_.BackBufferWidth  = width;
    d3dpp_.BackBufferHeight = height;



    release_mini_resources();
    check_succeded (pdevice_->Reset(&d3dpp_));
    set_mini_resources();
    //check_succeded (pdevice_->CreateVertexDeclaration(dwDeclSingle, &pvdeclsingle_));
    //check_succeded (pdevice_->CreateVertexDeclaration(dwDeclMulti, &pvdeclmulti_));
    check_succeded (pdevice_->SetStreamSource(1, psinglecolor_, 0, 0));
    
    unset_color();
    update_projection();
}

void d3d_vis::update_projection()
{
    RECT myrect;
    GetClientRect (hwnd_, &myrect);
    D3DXMATRIX proj;
    D3DXMatrixOrthoOffCenterRH (&proj, 0, (float)myrect.right, (float)myrect.bottom, 0, -1.0f, 1.0f);
    check_succeded (pdevice_->SetTransform (D3DTS_PROJECTION, &proj));
}


void d3d_vis::safe_release (IUnknown* p)
{
    if (p != NULL)
    {
        p->Release();
        p = NULL;
    }
}

void d3d_vis::check_succeded (HRESULT hr)
{
    assert (SUCCEEDED(hr));
    if (!SUCCEEDED(hr))
    {
        std::cerr << "Error!" << endl;
    }
}



void d3d_vis::release_mini_resources ()
{
    ULONG res;
    pdevice_->SetVertexDeclaration(NULL);
    res = pvdeclsingle_->Release();
    assert (res == 0);
    res = pvdeclmulti_->Release();
    assert (res == 0);
}

void d3d_vis::set_mini_resources ()
{
    D3DVERTEXELEMENT9 dwDeclSingle[] =
    {
        {0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {1,  0,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0} ,
        D3DDECL_END()
    };

    D3DVERTEXELEMENT9 dwDeclMulti[] =
    {
        {0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0,  12,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0},
        D3DDECL_END()
    };

    check_succeded (pdevice_->CreateVertexDeclaration(dwDeclSingle, &pvdeclsingle_));
    assert (pvdeclsingle_ != NULL);
    check_succeded (pdevice_->CreateVertexDeclaration(dwDeclMulti, &pvdeclmulti_));
    assert (pvdeclmulti_ != NULL);
}



visualizer *create_visualizer(draw_scope **ppscope) 
{
    d3d_vis *p = new d3d_vis (1024, 768);
    *ppscope = p;
    return p;
}

#pragma once

#include "../shared/coord.h"
#include "../shared/client.h"

typedef unsigned int vb_id;
typedef unsigned int ib_id;



struct b_vertex 
{
    float x, y, z;
    unsigned int color;
    static const unsigned int fvf = 0;
};

struct b_edge
{
    int v1, v2;
};

class visualizer
{
public:
    virtual ~visualizer () {};
    virtual vb_id   create_vb   (size_t)    = 0;
    virtual ib_id   create_ib   (size_t)    = 0;
    virtual void    free_vb     (vb_id)     = 0; 
    virtual void    free_ib     (ib_id)     = 0;
    
    virtual void draw_buffers (vb_id verts, size_t n_verts, ib_id inds, size_t n_inds)    = 0;
    virtual void draw_buffers (vb_id verts, size_t verts_offset, size_t n_verts, ib_id inds, size_t inds_offset, size_t n_inds)    = 0;
    virtual void draw_text  (coord<int>, const std::string&)                            = 0;

    virtual void draw_begin ()  = 0;
    virtual void draw_end   ()  = 0;

    virtual void draw_rect  (coord<int>, coord<int>) = 0;
    virtual void draw_rect_world  (coord<float>, coord<float>) = 0;
    virtual void draw_line  (coord<int>, coord<int>) = 0;
    virtual void set_bg_color  (unsigned char r, unsigned char g, unsigned char b) = 0;
    virtual void set_color  (unsigned char r, unsigned char g, unsigned char b) = 0;
    virtual void unset_color  () = 0;


    virtual b_vertex* lock_vb   (vb_id, size_t start, size_t size, unsigned int flags = 0) = 0;
    virtual void unlock_vb      (vb_id) = 0;

    virtual b_edge* lock_ib     (ib_id, size_t start, size_t size, unsigned int flags = 0) = 0;
    virtual void unlock_ib      (ib_id) = 0;

    virtual void resize (int width, int height) = 0;
    virtual void set_client (client* pcl) = 0;
};



#pragma once

class client
{
public:
    virtual ~client () {};

    virtual void on_mouse_move (int x, int y)               = 0;
    virtual void on_mouse_down (int x, int y, int button)   = 0;
    virtual void on_mouse_up   (int x, int y, int button)   = 0;
    virtual void on_key_down   (int key)                    = 0;
    virtual void on_wheel      (int delta)                  = 0;
    virtual void on_resize     (int width, int height)      = 0;
    virtual void on_paint      ()                           = 0;
};
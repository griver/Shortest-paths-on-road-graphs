#pragma once

#include "../shared/client.h"

class visualizer_client : public client
{
public:
    visualizer_client ();
    ~visualizer_client();

    void on_mouse_move    (int x, int y);
    void on_mouse_down    (int x, int y, int button);
    void on_mouse_up    (int x, int y, int button);
    void on_key_down    (int key);
    void on_resize        (int width, int height);
    void on_paint        ();
    
private:
    scoped_ptr<visualizer> pvis_;
    coord<int> mouse_coord;
    string str;
};
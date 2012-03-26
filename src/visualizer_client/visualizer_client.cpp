#include "stdafx.h"
#include "../shared/client.h"
#include "../shared/visualizer.h"
#include "visualizer_client.h"

visualizer *create_visualizer(client*);

visualizer_client::visualizer_client ()
: ready_ (false)
{
    visualizer *p = create_visualizer(this);
    pvis_.reset(p);
    assert (pvis_ != NULL);
    pvis_->set_color(0xff000000);
    pvis_->set_bg_color(0xffffffff);
    ready_ = true;
}

visualizer_client::~visualizer_client()
{
}

bool visualizer_client::is_ready ()
{
    return ready_;
}

void visualizer_client::on_mouse_move (int x, int y)
{
    mouse_coord = coord<int>(x, y);
    on_paint();
}

void visualizer_client::on_mouse_down (int x, int y, int button)
{

}

void visualizer_client::on_mouse_up (int x, int y, int button)
{

}

void visualizer_client::on_key_down (int key)
{
    if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z'))
        str += static_cast<char>(key);
    on_paint();
}

void visualizer_client::on_resize (int width, int height)
{

}

void visualizer_client::on_paint ()
{
    pvis_->draw_begin();
    pvis_->draw_text(coord<int>(0, 0), str);
    pvis_->draw_line(coord<int>(0, 0), mouse_coord);
    pvis_->draw_end();
}

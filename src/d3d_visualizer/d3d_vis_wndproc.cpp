#include "stdafx.h"
#include "d3d_vis.h"
#include "../shared/client.h"
#include "../shared/visualizer.h"

client* g_pClient = NULL;

void d3d_vis::set_client(client* pcl)
{
    g_pClient = pcl;
}

LRESULT CALLBACK d3d_vis_wndproc ( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{

    switch( message )
    {   
    case WM_DESTROY: 
        cout << "Good bye!\n";

        PostQuitMessage( 0 );
        return 0;
        break;
    }

    if (g_pClient != NULL)    
    {
        switch( message )
        {
        case WM_KEYDOWN:
            g_pClient->on_key_down(wparam);
            break;
        case WM_MOUSEMOVE:
            g_pClient->on_mouse_move(LOWORD(lparam), HIWORD(lparam));
            break;
        case WM_LBUTTONDOWN:
            g_pClient->on_mouse_down(LOWORD(lparam), HIWORD(lparam), 0);
            break;
        case WM_LBUTTONUP:
            g_pClient->on_mouse_up(LOWORD(lparam), HIWORD(lparam), 0);
            break;
        case WM_RBUTTONDOWN:
            g_pClient->on_mouse_down(LOWORD(lparam), HIWORD(lparam), 1);
            break;
        case WM_RBUTTONUP:
            g_pClient->on_mouse_up(LOWORD(lparam), HIWORD(lparam), 1);
            break;
        case WM_MOUSEWHEEL:
            g_pClient->on_wheel(GET_WHEEL_DELTA_WPARAM(wparam));
            break;
        case WM_SIZE:
            g_pClient->on_resize(LOWORD(lparam), HIWORD(lparam));
            break;
        case WM_PAINT:
            g_pClient->on_paint();
            break;
        } 
    }
    return DefWindowProc( hwnd, message, wparam, lparam );
}

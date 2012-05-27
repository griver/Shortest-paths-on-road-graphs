#include "stdafx.h"
#include "lib_test_client.h"

visualizer *create_visualizer(draw_scope **ppscope);


int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        std::cerr << "Not enough args" << endl;
        return 1;
    }

    draw_scope *pscope = NULL;
    scoped_ptr<visualizer> pvis (create_visualizer(&pscope));

    lib_test_client cl (argv[1], pvis.get(), pscope);
    pvis->set_client(&cl);


    MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( GetMessage( &msg, NULL, 0U, 0U/*, PM_REMOVE*/ ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
        }   
    }

    return 0;
}


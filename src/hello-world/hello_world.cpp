// hello-world.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../shared/client.h"
#include "../shared/visualizer.h"
#include "hello_client.h"




int main(int argc, char* argv[])
{
    {
        visualizer_client cl;

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
    }

    return 0;
}


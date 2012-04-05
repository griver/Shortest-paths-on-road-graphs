#include "stdafx.h"

//using namespace std;

// So you need the handle (HWND) for the
// console window for your app.

// Who knows why.  Maybe ya just
// want it.

///////////////////////////////
// In this example, I'm:
//  1) using the HWND of the console window
//     to get the HINSTANCE of the console window
//     (GetWindowLong() function)
//
//  2) using the HINSTANCE and HWND of the
//     console window to create a regular
//     window with a WndProc and a message
//     loop and all (WNDCLASS structure,
//     RegisterClass(), then CreateWindow())

// based off of MSDN KB article
// http://support.microsoft.com/kb/124103

// prototype for the WndProc of the window that
// we're gonna create.

/////////////////
// Normally, windowed apps would start
// at WinMain().
// However, remember that this is REALLY
// a console application, and we're just
// spawning a window out of it.
// This application starts at first line of main().
HWND create_window_from_console (int width, int height, WNDPROC WndProc)
{
    const size_t title_length = 1024;
    
    // console window exists as soon as app start

    // get its hwnd using combo of "GetConsoleWindowTitle()"
    // and "FindWindow()"

    // Why did i call this _REGION_A_? see end notes!
#pragma region _REGION_A_ //- get HWND of console

    char t[title_length];

    GetConsoleTitleA( t, title_length );     // retrieve the text
    // of the title bar of the console window

    //cout << "The title of the console window is:" << endl;
    //cout << t << endl << endl;

    HWND hwndConsole = FindWindowA( NULL, t );  // FindWindowA actually
    // can get you the HWND of a window, based
    // on the text in its title bar!

    // Let's check to see it worked.  If the console window
    // moves after the function call below, then we know
    // that hwndConsole is really a valid handle to the console
    // window!
    //MoveWindow( hwndConsole, 20, 20, 500, 500, true );  // test hwnd

#pragma endregion

#pragma region _REGION_B - get HINSTANCE and create a window!
    ////////////////////////
    // Getting the HINSTANCE given the HWND
    //
    // Want the HINSTANCE of a window, but
    // you only have its HWND?
    //
    // Here's how you generally get the HINSTANCE
    // of the console, based off of the HWND of
    // the console.
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwndConsole, GWLP_HINSTANCE);

    ////////////////////
    // Now I'm going to create an ACTUAL WINDOW.
    //
    // Note that you always need a HINSTANCE
    // to create a window, which is why I just
    // got it in the line just above here.
    WNDCLASS wc = {0};
    wc.hbrBackground =(HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hCursor = LoadCursor( hInstance, IDC_ARROW );
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = TEXT("peter");   // name of window class .. I choose peter
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (! RegisterClass( &wc ) )
    {
        return NULL;
        //return 1;   // ERR, SO QUIT
    }

    // Create a real live window!
    // (see http://bobobobo.wordpress.com/2008/01/31/how-to-create-a-basic-window-in-c/
    // for more details on basic windows)
    HWND hwndWindow = CreateWindow(  TEXT("peter"),
        TEXT("Visualizer"),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,   // if you make this hwndConsole, then
        // the console becomes this window's parent
        // Then, this window wouldn't get an
        // entry in the taskbar
        NULL,

        hInstance, NULL );

    ShowWindow( hwndWindow, SW_SHOWNORMAL );    // ShowWindow() on msdn
    UpdateWindow( hwndWindow );
#pragma endregion


    return hwndWindow;
}
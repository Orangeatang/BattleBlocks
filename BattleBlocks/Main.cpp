
// ------------------ Includes ------------------

#include "Stdafx.h"

#include "BattleBlocksGame.h"


// ------------------ Functions -----------------

// Entry point for the application - creates the window, initialises the game, DirectX and the 
// voxel engine
int WINAPI WinMain( HINSTANCE anInstance, HINSTANCE aPreviousInstance, LPSTR aCommandLine, int aShowCmd )
{
#ifdef _DEBUG
	// The following code is used for detecting memory leaks. If a memory leak is detected when the application exits,
	// the information will be printed out to the debug output window. Change the -1 in _CrtSetBreakAlloc( -1 ) to the
	// allocation number and run the code again... easy to tell if bits and pieces aren't being cleaned up
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );

	_CrtSetBreakAlloc( -1 );
#endif

    BattleBlocksGame* theGame = BattleBlocksGame::GetInstance();
    
    if( theGame->Initialise() )
    {
        theGame->Run();
    }
    else
    {
        MessageBox( NULL, L"Unable to initialise the game", L"ERROR", 0 );
    }
    
    theGame->Uninitialise();
    BattleBlocksGame::Cleanup();

    return 0;
}
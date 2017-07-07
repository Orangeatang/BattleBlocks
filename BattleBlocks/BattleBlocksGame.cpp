
// ---------------- Includes ----------------

#include "Stdafx.h"
#include "BattleBlocksGame.h"

#include "SystemInputProcessor.h"
#include "LocalPlayer.h"

#include <VoxelEngine.h>
#include <VEFirstPersonCamera.h>
#include <VEFreeCamera.h>
#include <VETerrainGenerator.h>
#include <VELightingManager.h>
#include <VEDirectionalLight.h>
#include <VEPointLight.h>
#include <VESpotLight.h>


// ----------------- Defines ----------------

#define SIXTY_FPS 0.016f


// ----------------- Statics ----------------

BattleBlocksGame* BattleBlocksGame::myInstance = NULL;


// ------------- Global Functions -----------

// Windows message processing
LRESULT CALLBACK WndProc( HWND aWnd, UINT aMessage, WPARAM aWParam, LPARAM aLParam )
{
	switch( aMessage )
	{
		case WM_DESTROY :
			PostQuitMessage( 0 );
			break;

		default :
			return BattleBlocksGame::GetInstance()->ProcessWindowsMessage( aWnd, aMessage, aWParam, aLParam );
			break;
	}

	return 0;
}


// ------------- Class Functions ------------

// Returns/creates the static game instance
BattleBlocksGame* BattleBlocksGame::GetInstance()
{
	if( myInstance == NULL )
	{
		myInstance = new BattleBlocksGame();
	}

	return myInstance;
}


// Cleans up the static instance of the game
void BattleBlocksGame::Cleanup()
{
	if( myInstance != NULL )
	{
		delete myInstance;
		myInstance = NULL;
	}
}


// Creates the game window and initialises the voxel engine
bool BattleBlocksGame::Initialise( int aScreenWidth /* 1024 */, int aScreenHeight /* 768 */ )
{
	myScreenWidth   = aScreenWidth;
	myScreenHeight  = aScreenHeight;

	std::wstring dataDirectory = L"Data/";

	// Get the instance for the current application
	myGameInstance = GetModuleHandle( NULL );
	if( myGameInstance == NULL )
	{
		return false;
	}

	// Register the application
	if( !RegisterGameWindow() )
	{
		return false;
	}

	// Create and show the game window
	if( !CreateGameWindow() )
	{
		return false;
	}
	
	// Initialise the engine
	VoxelEngine* voxelEngine = VoxelEngine::GetInstance();
	assert( voxelEngine != NULL );
	
	if( !voxelEngine->Initialise(myGameInstance, myWindowHandle, myScreenWidth, myScreenHeight, myFullScreen, myVsync, dataDirectory) )
	{
		return false;
	}
	
	// Create a player
	myLocalPlayer = new LocalPlayer();
	myLocalPlayer->Initialise( true );

	// Add a box voxel to the world
	voxelEngine->GetTerrainGenerator()->GenerateTerrain( 2, 2 );

	// Create an input processor
	myInputProcessor = new SystemInputProcessor();

	// Add a couple of lights in to the current scene
// 	VELightingManager* lightingManager = voxelEngine->GetLightingManager();
// 	assert( lightingManager != NULL );

// 	VEDirectionalLight* directionalLight = lightingManager->AddDirectionalLight();
// 	directionalLight->SetDirection( DirectX::XMFLOAT3(0.0f, -0.1f, 0.3f) );
// 	directionalLight->SetColour( DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) );
// 	directionalLight->SetCastsShadows( true );
 
// 	directionalLight = lightingManager->AddDirectionalLight();
// 	directionalLight->SetDirection( DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) );
// 	directionalLight->SetColour( DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f) );
// 	directionalLight->SetCastsShadows( true );

// 	VEPointLight* pointLight = lightingManager->AddPointLight();
// 	pointLight->SetPosition( DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) );
// 	pointLight->SetRadius( 50.0f );

// 	VESpotLight* spotLight = lightingManager->AddSpotLight();
// 	spotLight->SetPosition( DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f) );
// 	spotLight->SetDirection( DirectX::XMFLOAT3(0.0f, -0.5f, 0.0f) );
// 	spotLight->SetRadius( 50.0f );
// 	spotLight->SetDecayRate( 0.001f );
// 	spotLight->SetConeAngle( 5.0f );
// 	spotLight->SetCastsShadows( true );

	// Constrain the cursor to the window rectangle
	RECT windowRect;
	GetWindowRect(myWindowHandle, &windowRect); 
	ClipCursor(&windowRect); 

	return true;
}


// Cleans up the static instance
void BattleBlocksGame::Uninitialise()
{
	// Close the window
	if( myWindowHandle != NULL )
	{
		DestroyWindow( myWindowHandle );
		myWindowHandle = NULL;
	}

	if( myInputProcessor != NULL )
	{
		delete myInputProcessor;

		myInputProcessor = NULL;
	}

	if( myLocalPlayer != NULL )
	{
		delete myLocalPlayer;
		myLocalPlayer = NULL;
	}

	// Unregister the application
	if( myGameInstance != NULL )
	{
		UnregisterClass( myGameName, myGameInstance );
		myGameInstance = NULL;
	}	
	
	// Clean up the engine
	if( VoxelEngine* voxelEngine = VoxelEngine::GetInstance() )
	{
		voxelEngine->Uninitialise();
		VoxelEngine::Cleanup();
	}
}


// Runs the main update loop for the game
void BattleBlocksGame::Run()
{
	MSG     message;
	memset( &message, 0, sizeof(MSG) );
	bool    finished = false;
	
	// Start the frame timer
	StartFrameTimer();

	// Main game loop
	while( !finished )
	{
		// Dispatch any messages
		if( PeekMessage(&message, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &message );
			DispatchMessage( &message );
		}

		// If the message is WM_QUIT, end the game loop
		if( message.message == WM_QUIT )
		{
			finished = true;
		}
		// Otherwise perform an engine update
		else
		{
			myFrameTime += GetFrameTime();

			// Restrict the engine updates to 60fps
			if( myFrameTime > SIXTY_FPS )
			{
				// Update the engine
				VoxelEngine::GetInstance()->Update( SIXTY_FPS );

				// Update the input processor
				myInputProcessor->Update( myFrameTime );

				myFrameTime = 0.0f;
			}
		}
	}
}


// Deals with messages from Windows
LRESULT BattleBlocksGame::ProcessWindowsMessage( HWND aWnd, UINT aMessage, WPARAM aWParam, LPARAM aLParam )
{	
	return DefWindowProc( aWnd, aMessage, aWParam, aLParam );
}


// BattleBlocks construction
BattleBlocksGame::BattleBlocksGame() :
	myGameName( L"BattleBlocks" ),
	myGameInstance( NULL ),
	myWindowHandle( NULL ),
	myFullScreen( false ),
	myVsync( false ),
	myShowCursor( false ),
	myScreenWidth( 1024 ),
	myScreenHeight( 768 ),
	myInputProcessor( NULL ),
	myFrameTime( 0.0 ),
	myOldFrameTime( 0 ),
	myTicksPerSecond( 0.0 ),
	myLocalPlayer( NULL )
{
}


// Registers the BattleBlocks game window
bool BattleBlocksGame::RegisterGameWindow()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = myGameInstance;
	wcex.hIcon          = LoadIcon( myGameInstance, MAKEINTRESOURCE(IDI_APPLICATION) );
	wcex.hCursor        = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground  = (HBRUSH)( COLOR_WINDOW+1 );
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = myGameName;
	wcex.hIconSm        = LoadIcon( wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION) );

	if( !RegisterClassEx(&wcex) )
	{
		return false;
	}

	return true;
}


// Creates the window used for rendering
bool BattleBlocksGame::CreateGameWindow()
{
	DEVMODE screenSettings;
	DWORD   style;
	int     positionX;
	int     positionY;

	// Work out the screen settings
	if( myFullScreen )
	{
		memset( &screenSettings, 0, sizeof(DEVMODE) );
		screenSettings.dmSize       = sizeof(screenSettings);
		screenSettings.dmPelsWidth  = (unsigned long)myScreenWidth;
		screenSettings.dmPelsHeight = (unsigned long)myScreenHeight;
		screenSettings.dmBitsPerPel = 32;
		screenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings( &screenSettings, CDS_FULLSCREEN );

		positionX   = positionY = 0;
		style       = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
	}
	else
	{
		positionX = (GetSystemMetrics(SM_CXSCREEN) - myScreenWidth) / 2;
		positionY = (GetSystemMetrics(SM_CYSCREEN) - myScreenHeight) / 2;
		style     = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	// Create the window
	myWindowHandle = CreateWindowEx(
		WS_EX_APPWINDOW,
		myGameName,
		myGameName,
		style,
		positionX,
		positionY,
		myScreenWidth,
		myScreenHeight,
		NULL,
		NULL,
		myGameInstance,
		NULL
		);

	if( myWindowHandle == NULL )
	{
		return false;
	}

	// Display the window
	ShowWindow( myWindowHandle, SW_SHOW );
	SetForegroundWindow( myWindowHandle );
	SetFocus( myWindowHandle );

	// Hide the cursor
	ShowCursor( myShowCursor );

	return true;
}


// Starts/restarts high performance timer
void BattleBlocksGame::StartFrameTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency( &frequencyCount );

	myTicksPerSecond = (double)frequencyCount.QuadPart;
}


// Returns the number of milliseconds that have elapsed since the last time
// this function was called
float BattleBlocksGame::GetFrameTime()
{
	LARGE_INTEGER	currentTime;
	float			tickCount;

	QueryPerformanceCounter( &currentTime );

	tickCount		= (float)(currentTime.QuadPart - myOldFrameTime);
	myOldFrameTime	= currentTime.QuadPart;

	if( tickCount < 0 )
	{
		tickCount = 0;
	}

	return (float)((tickCount) / myTicksPerSecond);
}
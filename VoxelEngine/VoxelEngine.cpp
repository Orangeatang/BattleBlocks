
// ------------------ Includes ------------------

#include "Stdafx.h"
#include "VoxelEngine.h"

#include "VEDirectXInterface.h"
#include "VEDirectXInput.h"
#include "VEVoxelRenderManager.h"
#include "VEDeferredRenderManager.h"
#include "VEBasicCamera.h"
#include "VELightingManager.h"
#include "VEChunkManager.h"
#include "VEThreadManager.h"
#include "VEShaderManager.h"
#include "VETextureManager.h"
#include "VETerrainGenerator.h"

#include "VEPhysicsService.h"
#include "VEObjectService.h"


// ----------------- Namespaces -----------------

using namespace DirectX;


// ------------------ Statics -------------------

VoxelEngine* VoxelEngine::myVoxelEngine = NULL;


// ------------------ Functions -----------------

// Returns (creates if necessary) the static instance of the engine
VoxelEngine* VoxelEngine::GetInstance()
{
	if( myVoxelEngine == NULL )
	{
		myVoxelEngine = new VoxelEngine();
	}

	return myVoxelEngine;
}


// Cleans up the static instance
void VoxelEngine::Cleanup()
{
	if( myVoxelEngine != NULL )
	{
		delete myVoxelEngine;
		myVoxelEngine = NULL;
	}
}


// Initialises the voxel engine
bool VoxelEngine::Initialise( HINSTANCE anInstance, HWND aWindowHandle, int aScreenWidth, int aScreenHeight, bool isFullScreen, bool anEnableVsync, std::wstring aDataDirectory )
{
	myDataDirectory = aDataDirectory;

	myRenderInterface = new VEDirectXInterface();
	if( !myRenderInterface->Initialise( aWindowHandle, aScreenWidth, aScreenHeight, isFullScreen, anEnableVsync) )
	{
		return false;
	}

	myInputInterface = new VEDirectXInput();
	if( !myInputInterface->Initialise(anInstance, aWindowHandle, aScreenWidth, aScreenHeight) )
	{
		return false;
	}
	
	myTerrainGenerator	= new VETerrainGenerator();
	if( !myTerrainGenerator->Initialise() )
	{
		return false;
	}

	myThreadManager = new VEThreadManager();
	if( !myThreadManager->Initialise() )
	{
		return false;
	}

	myLightingManager	= new VELightingManager();
	myChunkManager		= new VEChunkManager();
	myShaderManager		= new VEShaderManager();	
	myTextureManager    = new VETextureManager();
	
	myObjectService		= new VEObjectService();
	myPhysicsService	= new VEPhysicsService();

	if( !CreateRenderer(RT_Deferred, aScreenWidth, aScreenHeight) )
	{
		return false;
	}

	return true;
}


// Releases the static instance of the engine
void VoxelEngine::Uninitialise()
{
	if( myInputInterface != NULL )
	{
		myInputInterface->Uninitialise();

		delete myInputInterface;
		myInputInterface = NULL;
	}

	if( myRenderer != NULL )
	{
		myRenderer->Uninitialise();
		
		delete myRenderer;
		myRenderer = NULL;
	}
	
	if( myRenderInterface != NULL )
	{
		myRenderInterface->Uninitialise();
		
		delete myRenderInterface;
		myRenderInterface = NULL;
	}

	if( myThreadManager != NULL )
	{
		myThreadManager->Uninitialise();

		delete myThreadManager;
		myThreadManager = NULL;
	}

	if( myLightingManager != NULL )
	{
		myLightingManager->Uninitialise();

		delete myLightingManager;
		myLightingManager = NULL;
	}

	if( myChunkManager != NULL )
	{
		myChunkManager->Uninitialise();
		
		delete myChunkManager;
		myChunkManager = NULL;
	}

	if( myShaderManager != NULL )
	{
		myShaderManager->Uninitialise();

		delete myShaderManager;
		myShaderManager = NULL;
	}

	if( myTextureManager != NULL )
	{
		myTextureManager->Uninitialise();

		delete myTextureManager;
		myTextureManager = NULL;
	}

	if( myObjectService != NULL )
	{
		delete myObjectService;
		myObjectService = NULL;
	}

	if( myTerrainGenerator != NULL )
	{
		myTerrainGenerator->Uninitialise();

		delete myTerrainGenerator;
		myTerrainGenerator = NULL;
	}

	if( myPhysicsService != NULL )
	{
		delete myPhysicsService;
		myPhysicsService = NULL;
	}
}


// Updates and renders the current scene
void VoxelEngine::Update( float anElapsedTime )
{
	myInputInterface->Update();
	
	myThreadManager->Update( anElapsedTime );
	myChunkManager->Update( anElapsedTime );
	
	myObjectService->Update( anElapsedTime );
	if( myPhysicsService->GetEnabled() )
	{
		myPhysicsService->Update( anElapsedTime );
	}
	
	myRenderer->RenderScene();
}


// Returns the type of camera being used
CameraType VoxelEngine::GetCameraType()
{
	if( myCamera != NULL )
	{
		return myCamera->GetType();
	}

	return CT_Max;
}


// Sets the draw mode for the render interface
void VoxelEngine::SetDrawMode( DrawMode aDrawMode )
{
	if( myRenderInterface != NULL )
	{
		myRenderInterface->SetDrawMode( aDrawMode );
	}
}


// Private construction - voxel engine should be accessed using the 'GetInstance' function
VoxelEngine::VoxelEngine() :
	myRenderer( NULL ),
	myCamera( NULL ),
	myLightingManager( NULL ),
	myChunkManager( NULL ),
	myThreadManager( NULL ),
	myShaderManager( NULL ),
	myTextureManager( NULL ),
	myObjectService( NULL ),
	myPhysicsService( NULL ),
	myRenderInterface( NULL ),
	myDataDirectory( L"" ),
	myInputInterface( NULL ),
	myTerrainGenerator( NULL )
{
}


// Creates and initialises the engine's renderer
bool VoxelEngine::CreateRenderer( RenderManagerType aRendererType, int aScreenWidth, int aScreenHeight )
{
	switch( aRendererType )
	{
		case RT_Voxel :
			myRenderer = new VEVoxelRenderManager();
			break;

		case RT_Deferred :
			myRenderer = new VEDeferredRenderManager();
			break;

		default :
			return false;
	}

	return myRenderer->Initialise( aScreenWidth, aScreenHeight );
}

#ifndef VOXEL_ENGINE_H
#define VOXEL_ENGINE_H


// ----------------- Includes -------------------

#include "VETypes.h"


// ------------ Forward Declarations ------------

class VERenderManager;
class VEBasicCamera;
class VEDirectXInterface;
class VEDirectXInput;
class VETerrainGenerator;
class VELightingManager;
class VEChunkManager;
class VEThreadManager;
class VEShaderManager;
class VETextureManager;
class VEObjectService;

class VEPhysicsService;


// ------------------ Classes -------------------

// The main class for the voxel engine, groups together all of the services, managers and rendering
// elements
class VoxelEngine
{
	public :

		// ---------- Public Functions ----------

		// Returns (creates if necessary) the static instance of the engine
		static VoxelEngine* GetInstance();

		// Cleans up the static instance
		static void         Cleanup();

		// Initialises the voxel engine
		bool                Initialise( HINSTANCE anInstance, HWND aWindowHandle, int aScreenWidth, int aScreenHeight, bool isFullScreen, bool anEnableVsync, std::wstring aDataDirectory );

		// Uninitialises the voxel engine
		void                Uninitialise();

		// Updates & renders the current scene
		void                Update( float anElapsedTime );


		// ------------- Accessors --------------

		VEDirectXInterface* GetRenderInterface()				{ return myRenderInterface; }

		VERenderManager*	GetRenderManager()					{ return myRenderer; }

		VEDirectXInput*		GetInputInterface()					{ return myInputInterface; }

		VELightingManager*	GetLightingManager()				{ return myLightingManager; }

		VEChunkManager*		GetChunkManager()					{ return myChunkManager; }

		VEThreadManager*	GetThreadManager()					{ return myThreadManager; }

		VEShaderManager*	GetShaderManager()					{ return myShaderManager; }

		VETextureManager*	GetTextureManager()					{ return myTextureManager; }

		VEObjectService*	GetObjectService()					{ return myObjectService; }

		VEPhysicsService*	GetPhysicsService()					{ return myPhysicsService; }

		VETerrainGenerator* GetTerrainGenerator()				{ return myTerrainGenerator; }
		
		std::wstring        GetDataDirectory()					{ return myDataDirectory; }

		VEBasicCamera*		GetCamera()							{ return myCamera; }
		void				SetCamera( VEBasicCamera* aCamera )	{ myCamera = aCamera; }

		CameraType			GetCameraType();

		void				SetDrawMode( DrawMode aDrawMode );


	private :

		// ---------- Private Functions ---------

		// Private construction - voxel engine should be accessed using the 'GetInstance' function
		VoxelEngine();

		// Creates and initialises the engine's renderer
		bool				CreateRenderer( RenderManagerType aRendererType, int aScreenWidth, int aScreenHeight );


		// ---------- Private Variables ---------
		
		static VoxelEngine*     myVoxelEngine;

		VERenderManager*		myRenderer;
		VEDirectXInterface*     myRenderInterface;
		VEDirectXInput*			myInputInterface;

		VEBasicCamera*          myCamera;

		VELightingManager*		myLightingManager;
		VEChunkManager*			myChunkManager;
		VEThreadManager*		myThreadManager;		
		VEShaderManager*		myShaderManager;
		VETextureManager*		myTextureManager;

		VEObjectService*		myObjectService;
		VEPhysicsService*		myPhysicsService;

		VETerrainGenerator*		myTerrainGenerator;

		std::wstring            myDataDirectory;
};


#endif // !VOXEL_ENGINE_H
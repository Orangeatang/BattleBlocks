
// ------------------- Includes -------------------

#include "Stdafx.h"
#include "SystemInputProcessor.h"

#include <VoxelEngine.h>
#include <VEFirstPersonCamera.h>
#include <VEDirectXInput.h>
#include <VERenderManager.h>
#include <VEChunkManager.h>
#include <VEChunk.h>


// ---------------- Class Functions ---------------

// Construction
SystemInputProcessor::SystemInputProcessor() :
	myMovementSpeed( 10.0f ),
	myFastMovementSpeed( 20.0f ),
	mySlowMovementSpeed( 1.0f )
{
}


// Updates the game/engine state based on mouse/keyboard input
void SystemInputProcessor::Update( float anElapsedTime )
{
	VEDirectXInput*			directInput = VoxelEngine::GetInstance()->GetInputInterface();
	VEFirstPersonCamera*	camera		= NULL;

	switch( VoxelEngine::GetInstance()->GetCameraType() )
	{
		case CT_Free :
		case CT_FirstPerson :
			camera = dynamic_cast<VEFirstPersonCamera*>( VoxelEngine::GetInstance()->GetCamera() );
			break;

		default :
			return;
	}

	// Draw mode
	if( directInput->IsKeyDown(DIK_F10) )
	{
		VoxelEngine::GetInstance()->SetDrawMode( DM_WireFrame );
	}
	else if( directInput->IsKeyDown(DIK_F11) )
	{
		VoxelEngine::GetInstance()->SetDrawMode( DM_BackFaceCulling );
	}
	else if( directInput->IsKeyDown(DIK_F9) )
	{
		SetChunkStyle( CS_Pyramid );
	}
	else if( directInput->IsKeyDown(DIK_F8) )
	{
		SetChunkStyle( CS_Sphere );
	}
	else if( directInput->IsKeyDown(DIK_F7) )
	{
		SetChunkStyle( CS_Box );
	}

	// Screen capture
	if( directInput->IsKeyDown(DIK_F1) )
	{
		VoxelEngine::GetInstance()->GetRenderManager()->CaptureFrame();
	}
}


// Changes the style of the chunk being rendered
void SystemInputProcessor::SetChunkStyle( ChunkStyle aStyle )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEChunk* activeChunk = chunkManager->GetChunks()[0];
	assert( activeChunk != NULL );

	activeChunk->ApplyStyle( aStyle );
}
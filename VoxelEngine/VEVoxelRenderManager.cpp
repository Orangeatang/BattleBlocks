
// ------------------ Includes ------------------

#include "Stdafx.h"
#include "VEVoxelRenderManager.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VEChunk.h"
#include "VEVoxelShader.h"
#include "VEBasicCamera.h"
#include "VEChunkManager.h"
#include "VEShaderManager.h"


// ----------------- Namespaces -----------------

using namespace DirectX;


// --------------- Class Functions --------------

// Construction
VEVoxelRenderManager::VEVoxelRenderManager() : 
	VERenderManager( RT_Voxel ),
	myDepthStencilTarget( NULL )
{
    memset( &myClearColour, 0, sizeof(float) * 4 );
}


// Initialises the renderer
bool VEVoxelRenderManager::Initialise( int aScreenWidth, int aScreenHeight )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	if( renderInterface == NULL )
	{
		return false;
	}

	VEShaderManager* shaderManager = VoxelEngine::GetInstance()->GetShaderManager();
	if( !shaderManager->LoadShader(VST_Voxel) )
	{
		return false;
	}

	myDepthStencilTarget = new VEDepthStencilTarget( aScreenWidth, aScreenHeight, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT );
	if( !renderInterface->CreateDepthStencilTarget(myDepthStencilTarget) )
	{
		return false;
	}

    return true;
}


// Cleans up the renderer
void VEVoxelRenderManager::Uninitialise()
{
	if( myDepthStencilTarget != NULL )
	{
		delete myDepthStencilTarget;
		myDepthStencilTarget = NULL;
	}
}


// Draws all of the voxels
void VEVoxelRenderManager::RenderScene()
{
    VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
    assert( renderInterface != NULL );

	VEBasicCamera* camera = VoxelEngine::GetInstance()->GetCamera();
	assert( camera != NULL );

	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEShaderManager* shaderManager = VoxelEngine::GetInstance()->GetShaderManager();
	assert( shaderManager != NULL );

	std::vector<VEChunk*> engineChunks = chunkManager->GetChunks();

	// Set the render targets
	renderInterface->SetBackBufferRenderTarget( myDepthStencilTarget->myDepthStencilView );
    renderInterface->Clear( myClearColour );

	VEShader* voxelShader = shaderManager->GetShader( VST_Voxel );
	assert( voxelShader != NULL );

	// Render the engine chunks
	for( unsigned int i = 0; i < engineChunks.size(); i++ )
	{			
		if( !engineChunks[i]->GetEnabled() )
		{
			continue;
		}

		engineChunks[i]->Prepare();

		// Populate the shader constants
		voxelShader->PopulateVertexShaderConstants( camera, NULL );
		voxelShader->PopulatePixelShaderConstants( camera, NULL );

		voxelShader->DrawIndexed( engineChunks[i]->GetIndexCount() );
	}
	
    renderInterface->PresentBuffer();
}
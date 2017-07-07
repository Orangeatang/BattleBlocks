
// ------------------------ Includes ------------------------

#include "Stdafx.h"
#include "VEDeferredRenderManager.h"

#include "VETypes.h"
#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VEQuadRenderer.h"
#include "VEChunkManager.h"
#include "VELightingManager.h"
#include "VEShaderManager.h"
#include "VETextureManager.h"
#include "VEChunk.h"
#include "VEBasicCamera.h"
#include "VEDirectionalLight.h"
#include "VEPointLight.h"
#include "VESpotLight.h"
#include "VESphere.h"
#include "VEShader.h"

#include "ScreenGrab.h"
#include "DDSTextureLoader.h"


// ----------------------- Namespaces -----------------------

using namespace DirectX;


// ------------------------ Defines -------------------------

#define RENDER_TARGET_COUNT 3


// --------------------- Class Functions --------------------

// Construction
VEDeferredRenderManager::VEDeferredRenderManager() :
	VERenderManager( RT_Deferred ),
	myColourTarget( NULL ),
	myNormalTarget( NULL ),
	myDepthRenderTarget( NULL ),
	myLightingRenderTarget( NULL ),
	myShadowRenderTarget( NULL ),
	myDepthStencilTarget( NULL ),
	mySSAORenderTarget( NULL ),
	myShadowDepthTarget( NULL ),
	myQuadRenderer( NULL ),
	mySphere( NULL ),
	myRandomNormalsTextureId( -1 )
{
}


// Initialises the render targets used by the deferred renderer
bool VEDeferredRenderManager::Initialise( int aScreenWidth, int aScreenHeight )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	if( renderInterface == NULL )
	{
		return false;
	}

	if( !InitialiseShaders() )
	{
		return false;
	}

	if( !InitialiseRenderTargets(renderInterface, aScreenWidth, aScreenHeight) )
	{
		return false;
	}

	if( !InitialiseDepthStencilStates(renderInterface, aScreenWidth, aScreenHeight) )
	{
		return false;
	}

	// Create the quad renderer
	myQuadRenderer = new VEQuadRenderer();
	if( !myQuadRenderer->Initialise() )
	{
		return false;
	}

	mySphere = new VESphere();
	if( !mySphere->CreateMesh() )
	{
		return false;
	}

	myClearColour[0] = 0.0f;
	myClearColour[1] = 0.0f;
	myClearColour[2] = 0.0f;
	myClearColour[3] = 0.0f;
	
	myDepthClearColour[0] = 1.0f;
	myDepthClearColour[1] = 1.0f;
	myDepthClearColour[2] = 1.0f;
	myDepthClearColour[3] = 1.0f;
	
	// Load the SSAO random normal texture
	VETextureManager* textureManager = VoxelEngine::GetInstance()->GetTextureManager();
	assert( textureManager != NULL );
	myRandomNormalsTextureId = textureManager->LoadTexture( L"RandomNormals.dds" );
	if( myRandomNormalsTextureId == -1 )
	{
		return false;
	}

	return true;
}


// Cleans up the memory used by the deferred renderer
void VEDeferredRenderManager::Uninitialise()
{
	if( myColourTarget != NULL )
	{
		delete myColourTarget;
		myColourTarget = NULL;
	}

	if( myNormalTarget != NULL )
	{
		delete myNormalTarget;
		myNormalTarget = NULL;
	}

	if( myDepthRenderTarget != NULL )
	{
		delete myDepthRenderTarget;
		myDepthRenderTarget = NULL;
	}

	if( mySSAORenderTarget != NULL )
	{
		delete mySSAORenderTarget;
		mySSAORenderTarget = NULL;
	}

	if( myLightingRenderTarget != NULL )
	{
		delete myLightingRenderTarget;
		myLightingRenderTarget = NULL;
	}

	if( myShadowRenderTarget != NULL )
	{
		delete myShadowRenderTarget;
		myShadowRenderTarget = NULL;
	}

	if( myDepthStencilTarget != NULL )
	{
		delete myDepthStencilTarget;
		myDepthStencilTarget = NULL;
	}

	if( myShadowDepthTarget != NULL )
	{
		delete myShadowDepthTarget;
		myShadowDepthTarget = NULL;
	}

	if( myQuadRenderer != NULL )
	{
		myQuadRenderer->Uninitialise();

		delete myQuadRenderer;
		myQuadRenderer = NULL;
	}

	if( mySphere != NULL )
	{
		delete mySphere;
		mySphere = NULL;
	}
}


// Uses deferred rendering to draw the scene
void VEDeferredRenderManager::RenderScene()
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	VEBasicCamera* camera = VoxelEngine::GetInstance()->GetCamera();
	assert( camera != NULL );

	VELightingManager* lightingManager = VoxelEngine::GetInstance()->GetLightingManager();
	assert( lightingManager != NULL );

	VEShaderManager* shaderManager = VoxelEngine::GetInstance()->GetShaderManager();
	assert( shaderManager != NULL );


	// ------- g-buffer -------

	ID3D11RenderTargetView* renderTargets[RENDER_TARGET_COUNT] = { myColourTarget->myRenderTarget, myNormalTarget->myRenderTarget, myDepthRenderTarget->myRenderTarget };
	renderInterface->SetRenderTargets( renderTargets, RENDER_TARGET_COUNT, myDepthStencilTarget->myDepthStencilView );
	renderInterface->Clear( myClearColour );

	// Clear the render targets and the depth buffer
	ClearGBuffer( renderInterface, shaderManager );

	// Render the g-buffer
	RenderGBuffer( renderInterface, camera, shaderManager );


	// --------- SSAO -----------

	// SSAO pass
	RenderSSAO( renderInterface, camera, renderTargets, shaderManager );


	// ------- lighting ---------

	// Render directional lights to the lighting buffer
	RenderDirectionalLights( renderInterface, camera, lightingManager, renderTargets, shaderManager );

	// Render point lights to the lighting buffer
	RenderPointLights( renderInterface, camera, lightingManager, shaderManager );

	// Render spot lights to the lighting buffer
	RenderSpotLights( renderInterface, camera, lightingManager, renderTargets, shaderManager );


	// --------- final --------

	// Clear the back buffer
	renderInterface->ClearRenderTargets();
	renderInterface->SetRenderTargets( renderTargets, RENDER_TARGET_COUNT, NULL );
	renderInterface->SetBackBufferRenderTarget( NULL );
	renderInterface->Clear( myClearColour, false );

	// Blend the light buffer with the colour buffer
	RenderFinalFrame( renderInterface, camera, shaderManager );

	if( myCaptureFrame )
	{
		SaveBuffersToFile( renderInterface );
	}

	// Display the back buffer
	renderInterface->PresentBuffer();
}


// Initialises all of the shaders
bool VEDeferredRenderManager::InitialiseShaders()
{
	VEShaderManager* shaderManager = VoxelEngine::GetInstance()->GetShaderManager();
	assert( shaderManager != NULL );

	if( !shaderManager->LoadShader(VST_ClearGBuffer) )
	{
		return false;
	}

	if( !shaderManager->LoadShader(VST_RenderGBuffer) )
	{
		return false;
	}

	if( !shaderManager->LoadShader(VST_DirectionalLight) )
	{
		return false;
	}

	if( !shaderManager->LoadShader(VST_PointLight) )
	{
		return false;
	}

	if( !shaderManager->LoadShader(VST_SpotLight) )
	{
		return false;
	}

	if( !shaderManager->LoadShader(VST_ShadowMap) )
	{
		return false;
	}

	if( !shaderManager->LoadShader(VST_FinalBlend) )
	{
		return false;
	}

	if( !shaderManager->LoadShader(VST_SSAO) )
	{
		return false;
	}

	return true;
}


// Initialises all of the render targets
bool VEDeferredRenderManager::InitialiseRenderTargets( VEDirectXInterface* aRenderInterface, int aScreenWidth, int aScreenHeight )
{
	myColourTarget = new VERenderTarget( aScreenWidth, aScreenHeight, DXGI_FORMAT_R8G8B8A8_UNORM );
	if( !aRenderInterface->CreateRenderTarget(myColourTarget, true) )
	{
		return false;
	}

	myNormalTarget = new VERenderTarget( aScreenWidth, aScreenHeight, DXGI_FORMAT_R8G8B8A8_UNORM );
	if( !aRenderInterface->CreateRenderTarget(myNormalTarget, true) )
	{
		return false;
	}

	myDepthRenderTarget = new VERenderTarget( aScreenWidth, aScreenHeight, DXGI_FORMAT_R32_FLOAT );
	if( !aRenderInterface->CreateRenderTarget(myDepthRenderTarget, true) )
	{
		return false;
	}

	mySSAORenderTarget = new VERenderTarget( aScreenWidth, aScreenHeight, DXGI_FORMAT_R8G8B8A8_UNORM );
	if( !aRenderInterface->CreateRenderTarget(mySSAORenderTarget, true) )
	{
		return false;
	}

	myLightingRenderTarget = new VERenderTarget( aScreenWidth, aScreenHeight, DXGI_FORMAT_R16G16B16A16_FLOAT );
	if( !aRenderInterface->CreateRenderTarget(myLightingRenderTarget, true) )
	{
		return false;
	}

	myShadowRenderTarget = new VERenderTarget( (int)SHADOW_MAP_SIZE, (int)SHADOW_MAP_SIZE, DXGI_FORMAT_R32_FLOAT );
	if( !aRenderInterface->CreateRenderTarget(myShadowRenderTarget, true) )
	{
		return false;
	}	

	return true;
}


// Initialises all of the depth stencil states
bool VEDeferredRenderManager::InitialiseDepthStencilStates( VEDirectXInterface* aRenderInterface, int aScreenWidth, int aScreenHeight )
{
	// Create a depth stencil target for the shadow rendering (it needs to be the same size as the shadow map target)
	myShadowDepthTarget = new VEDepthStencilTarget( (int)SHADOW_MAP_SIZE, (int)SHADOW_MAP_SIZE, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT );
	if( !aRenderInterface->CreateDepthStencilTarget(myShadowDepthTarget) )
	{
		return false;
	}

	// Create the default depth stencil target
	myDepthStencilTarget = new VEDepthStencilTarget( aScreenWidth, aScreenHeight, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, true );
	if( !aRenderInterface->CreateDepthStencilTarget(myDepthStencilTarget) )
	{
		return false;
	}

	return true;
}


// Clears the depth stencil buffer and the render targets used in the geometry shader
void VEDeferredRenderManager::ClearGBuffer( VEDirectXInterface* aRenderInterface, VEShaderManager* aShaderManager )
{
	VEShader* shader = aShaderManager->GetShader( VST_ClearGBuffer );
	assert( shader != NULL );

	// Clear the render targets
	myQuadRenderer->DrawQuad( shader );
}


// Renders all of the chunks to the g-buffer
void VEDeferredRenderManager::RenderGBuffer( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VEShaderManager* aShaderManager )
{
	VEChunkManager* chunkManager	= VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEShader* gBufferShader = aShaderManager->GetShader( VST_RenderGBuffer );
	assert( gBufferShader != NULL );

	aRenderInterface->EnableDepthTesting();

	gBufferShader->PopulateVertexShaderConstants( aCamera, NULL );
	gBufferShader->PopulatePixelShaderConstants( aCamera, NULL );

	// Draw the chunks to the colour, normal & depth render targets
	std::vector<VEChunk*> engineChunks = chunkManager->GetChunks();
	for( unsigned int i = 0; i < engineChunks.size(); i++ )
	{			
		if( !engineChunks[i]->GetEnabled() )
		{
			continue;
		}

		engineChunks[i]->Prepare();	
		gBufferShader->DrawIndexed( engineChunks[i]->GetIndexCount() );
	}

	aRenderInterface->DisableDepthTesting();
}


// Renders an SSAO buffer, used in the final blend shader
void VEDeferredRenderManager::RenderSSAO( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, ID3D11RenderTargetView** someRenderTargets, VEShaderManager* aShaderManager )
{
	VEShader* ssaoShader = aShaderManager->GetShader( VST_SSAO );
	assert( ssaoShader != NULL );

	VETextureManager* textureManager = VoxelEngine::GetInstance()->GetTextureManager();
	assert( textureManager != NULL );

	VETexture* randomNormals = textureManager->GetTexture( myRandomNormalsTextureId );
	assert( randomNormals != NULL );

	someRenderTargets[0] = mySSAORenderTarget->myRenderTarget;
	someRenderTargets[1] = NULL;
	someRenderTargets[2] = NULL;

	aRenderInterface->SetRenderTargets( someRenderTargets, 3, myDepthStencilTarget->myDepthStencilView );
	aRenderInterface->Clear( myClearColour, false );

	aRenderInterface->DisableDepthTesting();
	aRenderInterface->DisableAlphaBlending();

	ID3D11ShaderResourceView*	shaderResources[]	= { myNormalTarget->myShaderResource, myDepthRenderTarget->myShaderResource, randomNormals->myTexture };
	ID3D11SamplerState*			samplerStates[]		= { myNormalTarget->mySamplerState, myDepthRenderTarget->mySamplerState, randomNormals->mySampler };

	ssaoShader->SetShaderResources( shaderResources, 3 );
	ssaoShader->SetSamplerStates( samplerStates, 3 );

	ssaoShader->PopulateVertexShaderConstants( aCamera, NULL );
	ssaoShader->PopulatePixelShaderConstants( aCamera, NULL );

	myQuadRenderer->DrawQuad( ssaoShader );

	ssaoShader->ClearResources();
	ssaoShader->ClearSamplers();
}


// Renders shadows from the light's perspective to a shadow render target. This function assumes you have set up the appropriate render
// targets and cleared them
void VEDeferredRenderManager::RenderShadowMap( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELight* aLight, VEShaderManager* aShaderManager )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );
	assert( aLight != NULL );

	VEShader* shadowMapShader = aShaderManager->GetShader( VST_ShadowMap );
	assert( shadowMapShader != NULL );

	// Enable alpha blending and disable depth testing
	aRenderInterface->EnableDepthTesting();
	
	shadowMapShader->PopulateVertexShaderConstants( aCamera, aLight );
	shadowMapShader->PopulatePixelShaderConstants( aCamera, aLight );

	const std::vector<VEChunk*> chunks = chunkManager->GetChunks();
	for( unsigned int i = 0; i < chunks.size(); i++ )
	{
		chunks[i]->Prepare();

		shadowMapShader->DrawIndexed( chunks[i]->GetIndexCount() );
	}

	// Disable alpha blending
	aRenderInterface->DisableDepthTesting();
}


// Renders all of the directional lights to the lighting buffer
void VEDeferredRenderManager::RenderDirectionalLights( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELightingManager* aLightingManager, ID3D11RenderTargetView** someRenderTargets, VEShaderManager* aShaderManager )
{
	VEShader* directionalShader = aShaderManager->GetShader( VST_DirectionalLight );
	assert( directionalShader != NULL );

	// Clear the lighting buffer
	someRenderTargets[0] = myLightingRenderTarget->myRenderTarget;
	someRenderTargets[1] = NULL;
	someRenderTargets[2] = NULL;
	
	aRenderInterface->SetRenderTargets( someRenderTargets, RENDER_TARGET_COUNT, NULL );
	aRenderInterface->Clear( myClearColour, false );

	const std::vector<VEDirectionalLight*> lights = aLightingManager->GetDirectionalLights();
	if( lights.size() == 0 )
	{
		return;
	}

	// Loop through the directional lights
	for( unsigned int i = 0; i < lights.size(); i++ )
	{
		// If the current light casts shadows, render a shadow map from the light's perspective
		if( lights[i]->GetCastsShadows() )
		{
			someRenderTargets[0] = myShadowRenderTarget->myRenderTarget;
			someRenderTargets[1] = NULL;
			someRenderTargets[2] = NULL;
			
			aRenderInterface->SetRenderTargets( someRenderTargets, RENDER_TARGET_COUNT, myShadowDepthTarget->myDepthStencilView );
			aRenderInterface->SetViewport( myShadowDepthTarget->myWidth, myShadowDepthTarget->myHeight );
			aRenderInterface->Clear( myDepthClearColour, true );

			RenderShadowMap( aRenderInterface, aCamera, lights[i], aShaderManager );
		}

		// Reset the render target
		someRenderTargets[0] = myLightingRenderTarget->myRenderTarget;
		someRenderTargets[1] = NULL;
		someRenderTargets[2] = NULL;
		aRenderInterface->SetRenderTargets( someRenderTargets, RENDER_TARGET_COUNT, NULL );
		aRenderInterface->SetViewport( myLightingRenderTarget->myWidth, myLightingRenderTarget->myHeight );
		
		// Set up the lighting resources
		ID3D11SamplerState*		  samplerStates[]		= { myNormalTarget->mySamplerState, myDepthRenderTarget->mySamplerState, myShadowRenderTarget->mySamplerState };
		ID3D11ShaderResourceView* lightingResources[]	= { myNormalTarget->myShaderResource, myDepthRenderTarget->myShaderResource, myShadowRenderTarget->myShaderResource };
		directionalShader->SetShaderResources( lightingResources, 3 );
		directionalShader->SetSamplerStates( samplerStates, 3 );
		directionalShader->PopulatePixelShaderConstants( aCamera, lights[i] );
		directionalShader->PopulateVertexShaderConstants( aCamera, lights[i] );

		// Now accumulate light in the lighting buffer
		aRenderInterface->EnableAlphaBlending();
		aRenderInterface->DisableDepthTesting();
		myQuadRenderer->DrawQuad( directionalShader );

		// Unbind the resources from the lighting shader
		directionalShader->ClearSamplers();
		directionalShader->ClearResources();

		aRenderInterface->DisableAlphaBlending();
	}
}


// Renders all of the point lights to the lighting buffer
void VEDeferredRenderManager::RenderSpotLights( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELightingManager* aLightingManager, ID3D11RenderTargetView** someRenderTargets, VEShaderManager* aShaderManager )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEShader* spotShader = aShaderManager->GetShader( VST_SpotLight );
	assert( spotShader != NULL );

	const std::vector<VESpotLight*> lights = aLightingManager->GetSpotLights();
	if( lights.size() == 0 )
	{
		return;
	}

	XMFLOAT3 cameraPosition = aCamera->GetPosition();

	for( unsigned int i = 0; i < lights.size(); i++ )
	{
		// If the current light casts shadows, render a shadow map from the light's perspective
		if( lights[i]->GetCastsShadows() )
		{
			someRenderTargets[0] = myShadowRenderTarget->myRenderTarget;
			someRenderTargets[1] = NULL;
			someRenderTargets[2] = NULL;

			aRenderInterface->SetRenderTargets( someRenderTargets, RENDER_TARGET_COUNT, myShadowDepthTarget->myDepthStencilView );
			aRenderInterface->SetViewport( myShadowDepthTarget->myWidth, myShadowDepthTarget->myHeight );
			aRenderInterface->Clear( myDepthClearColour, true );

			RenderShadowMap( aRenderInterface, aCamera, lights[i], aShaderManager );
		}

		// Reset the render target
		someRenderTargets[0] = myLightingRenderTarget->myRenderTarget;
		someRenderTargets[1] = NULL;
		someRenderTargets[2] = NULL;
		aRenderInterface->SetRenderTargets( someRenderTargets, RENDER_TARGET_COUNT, NULL );
		aRenderInterface->SetViewport( myLightingRenderTarget->myWidth, myLightingRenderTarget->myHeight );

		// Add the sphere's geometry to the input assembler
		mySphere->Prepare();

		// Set the shader resources for the spot light shader
		ID3D11ShaderResourceView*	lightingResources[] = { myNormalTarget->myShaderResource, myDepthRenderTarget->myShaderResource, myShadowRenderTarget->myShaderResource };
		ID3D11SamplerState*			samplerStates[]		= { myNormalTarget->mySamplerState, myDepthRenderTarget->mySamplerState, myShadowRenderTarget->mySamplerState };
		spotShader->SetSamplerStates( samplerStates, 3 );
		spotShader->SetShaderResources( lightingResources, 3 );

		// Work out whether we are inside of the light's sphere, or looking at it from a distance
		XMFLOAT3	lightPosition = lights[i]->GetPosition();
		XMFLOAT3	cameraToLight( cameraPosition.x - lightPosition.x, cameraPosition.y - lightPosition.y, cameraPosition.z - lightPosition.z );
		float		distanceToLight = sqrt( (cameraToLight.x * cameraToLight.x) + (cameraToLight.y * cameraToLight.y) + (cameraToLight.z * cameraToLight.z) );

		// Use front face culling if we are inside the sphere
		if( distanceToLight < lights[i]->GetRadius() )
		{
			aRenderInterface->SetDrawMode( DM_FrontFaceCulling );
		}
		else
		{
			aRenderInterface->SetDrawMode( DM_BackFaceCulling );
		}

		aRenderInterface->EnableAlphaBlending();
		aRenderInterface->DisableDepthTesting();

		// Draw the sphere to touch the pixels that require spot-light lighting calculations
		spotShader->PopulateVertexShaderConstants( aCamera, lights[i] );
		spotShader->PopulatePixelShaderConstants( aCamera, lights[i] );
		spotShader->DrawIndexed( mySphere->GetIndexCount() );

		aRenderInterface->SetDrawMode( DM_BackFaceCulling );
		aRenderInterface->DisableAlphaBlending();

		// Unbind the shader resources
		spotShader->ClearResources();
		spotShader->ClearSamplers();
	}
}


// Renders all of the point lights to the lighting buffer
void VEDeferredRenderManager::RenderPointLights( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELightingManager* aLightingManager, VEShaderManager* aShaderManager )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEShader* pointShader = aShaderManager->GetShader( VST_PointLight );
	assert( pointShader != NULL );

	const std::vector<VEPointLight*> lights = aLightingManager->GetPointLights();
	if( lights.size() == 0 )
	{
		return;
	}

	XMFLOAT3 cameraPosition = aCamera->GetPosition();

	// Enable alpha blending and disable depth testing
	aRenderInterface->DisableDepthTesting();
	aRenderInterface->EnableAlphaBlending();

	// Set up the lighting resources
	ID3D11ShaderResourceView*	lightingResources[] = { myNormalTarget->myShaderResource, myDepthRenderTarget->myShaderResource };
	ID3D11SamplerState*			lightingSamplers[] = { myNormalTarget->mySamplerState, myDepthRenderTarget->mySamplerState };
	pointShader->SetShaderResources( lightingResources, 2 );
	pointShader->SetSamplerStates( lightingSamplers, 2 );

	for( unsigned int i = 0; i < lights.size(); i++ )
	{
		assert( lights[i] != NULL );

		// Add the sphere geometry to the input assembler
		mySphere->Prepare();

		// Calculate the distance from the camera to the light position.
		// If we're inside the sphere we want to cull the front faces, not the back faces
		XMFLOAT3 lightPosition	= lights[i]->GetPosition();
		XMFLOAT3 cameraToLight( cameraPosition.x - lightPosition.x, cameraPosition.y - lightPosition.y, cameraPosition.z - lightPosition.z );
		float	 distanceToLight = sqrt( cameraToLight.x * cameraToLight.x + cameraToLight.y * cameraToLight.y + cameraToLight.z * cameraToLight.z );

		if( distanceToLight < lights[i]->GetRadius() )
		{
			aRenderInterface->SetDrawMode( DM_FrontFaceCulling );
		}
		else
		{
			aRenderInterface->SetDrawMode( DM_BackFaceCulling );
		}

		// Render the sphere to the lighting buffer
		pointShader->PopulateVertexShaderConstants( aCamera, lights[i] );
		pointShader->PopulatePixelShaderConstants( aCamera, lights[i] );
		pointShader->DrawIndexed( mySphere->GetIndexCount() );
	}

	// Unbind the shader resources
	pointShader->ClearSamplers();
	pointShader->ClearResources();

	aRenderInterface->SetDrawMode( DM_BackFaceCulling );
	aRenderInterface->DisableAlphaBlending();
}


// Blends the lighting buffer with the colour buffer
void VEDeferredRenderManager::RenderFinalFrame( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VEShaderManager* aShaderManager )
{
	VEShader* finalBlend = aShaderManager->GetShader( VST_FinalBlend );
	assert( finalBlend != NULL );

	// Set up the light blending shader
	ID3D11SamplerState*			samplerStates[]		= { myColourTarget->mySamplerState, myLightingRenderTarget->mySamplerState, mySSAORenderTarget->mySamplerState };
	ID3D11ShaderResourceView*	blendResources[]	= { myColourTarget->myShaderResource, myLightingRenderTarget->myShaderResource, mySSAORenderTarget->myShaderResource };
	finalBlend->PopulateVertexShaderConstants( aCamera, NULL );
	finalBlend->PopulatePixelShaderConstants( aCamera, NULL );
	finalBlend->SetShaderResources( blendResources, 3 );
	finalBlend->SetSamplerStates( samplerStates, 3 );

	// Draw the final frame
	aRenderInterface->DisableDepthTesting();
	myQuadRenderer->DrawQuad( finalBlend );

	// Unbind the shader resources
	finalBlend->ClearResources();
	finalBlend->ClearSamplers();
}


// Saves the colour, normal and depth buffers to texture files
void VEDeferredRenderManager::SaveBuffersToFile( VEDirectXInterface* aRenderInterface )
{
	assert( aRenderInterface != NULL );

	ID3D11DeviceContext* deviceContext = aRenderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	DirectX::SaveDDSTextureToFile( deviceContext, myColourTarget->myRenderTargetTexture, L"ColourTarget.dds" );
	DirectX::SaveDDSTextureToFile( deviceContext, myNormalTarget->myRenderTargetTexture, L"NormalTarget.dds" );
	DirectX::SaveDDSTextureToFile( deviceContext, myDepthRenderTarget->myRenderTargetTexture, L"DepthTarget.dds" );
	DirectX::SaveDDSTextureToFile( deviceContext, myLightingRenderTarget->myRenderTargetTexture, L"LightTarget.dds" );
	
	myCaptureFrame = false;
}
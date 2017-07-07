
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VESpotLightShader.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VESpotLight.h"
#include "VEBasicCamera.h"
#include "VELightingManager.h"


// ---------------------- Namespaces --------------------

using namespace DirectX;


// ------------------- Class Functions ------------------


// Construction
VESpotLightShader::VESpotLightShader() : 
	VEShader( sizeof(MatrixBuffer), sizeof(LightingConstants) )
{
	myVertexShaderFile = L"DeferredPointLightShader.vs";
	myPixelShaderFile  = L"DeferredSpotLightShader.ps";
}


// Fills the vertex shader constant buffer
bool VESpotLightShader::PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight )
{
	HRESULT						result;
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	MatrixBuffer*				dataBuffer;
	VESpotLight*				SpotLight = dynamic_cast<VESpotLight*>( aLight );
	if( SpotLight == NULL )
	{
		return false;
	}

	// Offset the world matrix by the light position and radius
	float		lightRadius		= SpotLight->GetRadius();
	XMFLOAT3	lightPosition	= SpotLight->GetPosition();
	XMMATRIX	world			= XMMatrixMultiply( XMMatrixScaling(lightRadius, lightRadius, lightRadius), XMMatrixTranslation(lightPosition.x, lightPosition.y, lightPosition.z) );
	XMMATRIX	view			= XMLoadFloat4x4( &aCamera->GetView() );
	XMMATRIX	projection		= XMLoadFloat4x4( &aCamera->GetProjection() );

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Grab the constant buffer so we can write to it
	result = deviceContext->Map( myVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	if( FAILED(result) )
	{
		return false;
	}

	// Get a pointer to the buffer
	dataBuffer = (MatrixBuffer*)mappedResource.pData;

	// Copy the matrices in to the constant buffer
	dataBuffer->myWorld			= XMMatrixTranspose( world );
	dataBuffer->myView			= XMMatrixTranspose( view );
	dataBuffer->myProjection	= XMMatrixTranspose( projection );

	// Release the constant buffer lock
	deviceContext->Unmap( myVertexConstantBuffer, 0 );

	// Set the constant buffer for the vertex shader
	deviceContext->VSSetConstantBuffers( 0, 1, &myVertexConstantBuffer );

	return true;
}


// Fills the pixel shader constant buffer
bool VESpotLightShader::PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight )
{
	assert( aCamera != NULL );
	assert( aLight != NULL );
	assert( aLight->GetType() == LT_Spot );

	HRESULT						result;
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	LightingConstants*			lightingBuffer;
	VESpotLight*				spotLight = dynamic_cast<VESpotLight*>( aLight );

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	VELightingManager* lightingManager = VoxelEngine::GetInstance()->GetLightingManager();
	assert( lightingManager != NULL );
	XMFLOAT4 ambientLight = lightingManager->GetAmbientColour();

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Grab the constant buffer so we can write to it
	result = deviceContext->Map( myPixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	if( FAILED(result) )
	{
		return false;
	}

	// Calculate the inverse view-projection matrix
	XMMATRIX viewProj						= XMMatrixMultiply( XMLoadFloat4x4(&aCamera->GetView()), XMLoadFloat4x4(&aCamera->GetProjection()) );
	XMMATRIX inverseViewProj				= XMMatrixInverse( NULL, viewProj );

	// Calculate the light's view/projection matrix
	XMMATRIX lightVP						= XMMatrixMultiply( XMLoadFloat4x4(&aLight->GetViewMatrix()), XMLoadFloat4x4(&aLight->GetProjectionMatrix()) );

	// Get a pointer to the buffer
	lightingBuffer = (LightingConstants*)mappedResource.pData;

	// Copy the matrices in to the constant buffer
	lightingBuffer->myLightPosition			= spotLight->GetPosition();
	lightingBuffer->myLightRadius			= spotLight->GetRadius();
	lightingBuffer->myLightColour			= spotLight->GetColour();
	lightingBuffer->myLightDecay			= spotLight->GetDecayRate();
	lightingBuffer->myLightDirection		= spotLight->GetDirection();
	lightingBuffer->myLightConeAngle		= cos(spotLight->GetConeAngle());
	lightingBuffer->myApplyShadows			= aLight->GetCastsShadows() ? 1.0f : 0.0f;
	lightingBuffer->myInverseViewProjection = XMMatrixTranspose(inverseViewProj);
	lightingBuffer->myLightViewProjection	= XMMatrixTranspose( lightVP );

	// Release the constant buffer lock
	deviceContext->Unmap( myPixelConstantBuffer, 0 );

	// Set the constant buffer for the vertex shader
	deviceContext->PSSetConstantBuffers( 0, 1, &myPixelConstantBuffer );

	return true;
}


// Creates the data layout used to send vertex information to the vertex shader
bool VESpotLightShader::CreateDataLayout( ID3D10Blob* aVertexShader )
{
	const int                   itemCount = 2;
	D3D11_INPUT_ELEMENT_DESC    dataLayout[itemCount];
	HRESULT                     result;

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	// Position
	dataLayout[0].SemanticName          = "POSITION";
	dataLayout[0].SemanticIndex         = 0;
	dataLayout[0].Format                = DXGI_FORMAT_R32G32B32_FLOAT;
	dataLayout[0].InputSlot             = 0;
	dataLayout[0].AlignedByteOffset     = 0;
	dataLayout[0].InputSlotClass        = D3D11_INPUT_PER_VERTEX_DATA;
	dataLayout[0].InstanceDataStepRate  = 0;

	// Normal
	dataLayout[1].SemanticName          = "NORMAL";
	dataLayout[1].SemanticIndex         = 0;
	dataLayout[1].Format                = DXGI_FORMAT_R32G32B32_FLOAT;
	dataLayout[1].InputSlot             = 0;
	dataLayout[1].AlignedByteOffset     = D3D11_APPEND_ALIGNED_ELEMENT;
	dataLayout[1].InputSlotClass        = D3D11_INPUT_PER_VERTEX_DATA;
	dataLayout[1].InstanceDataStepRate  = 0;

	// Create the data layout
	result = renderInterface->GetDevice()->CreateInputLayout(
		dataLayout,
		itemCount,
		aVertexShader->GetBufferPointer(),
		aVertexShader->GetBufferSize(),
		&myVertexShaderInputLayout
		);

	if( FAILED(result) )
	{
		return false;
	}

	return true;
}
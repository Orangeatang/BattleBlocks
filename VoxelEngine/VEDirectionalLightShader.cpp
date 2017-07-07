
// ------------------- Includes -------------------

#include "Stdafx.h"
#include "VEDirectionalLightShader.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VERenderManager.h"
#include "VEBasicCamera.h"
#include "VEDirectionalLight.h"
#include "VELightingManager.h"


// ------------------- Namespaces -----------------

using namespace DirectX;


// ---------------- Class Functions ---------------

// Construction
VEDirectionalLightShader::VEDirectionalLightShader() : VEShader( 0, sizeof(LightingConstants) )
{
	myVertexShaderFile = L"DeferredDirectionalLightShader.vs";
	myPixelShaderFile  = L"DeferredDirectionalLightShader.ps";
}


// Fills the pixel shader constant buffer
bool VEDirectionalLightShader::PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight )
{
	assert( aLight != NULL && aLight->GetType() == LT_Directional );

	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	LightingConstants*			lightingBuffer;
	HRESULT						result;
	VEDirectionalLight*			directionalLight = dynamic_cast<VEDirectionalLight*>( aLight );
	assert( directionalLight != NULL );

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	VERenderManager* renderManager = VoxelEngine::GetInstance()->GetRenderManager();
	assert( renderManager != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Grab the lighting constant buffer
	result = deviceContext->Map( myPixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	if( FAILED(result) )
	{
		return false;
	}

	// Get the direction of the light
	VEDirectionalLight* light			= dynamic_cast<VEDirectionalLight*>( aLight );
	XMFLOAT3			lightDirection	= light->GetDirection();

	// Fill in the lighting buffer
	XMFLOAT3 lightColour					= directionalLight->GetColour();
	lightingBuffer							= (LightingConstants*)mappedResource.pData;
	lightingBuffer->myCameraPosition		= aCamera->GetPosition();
	lightingBuffer->myLightColour			= XMFLOAT4( lightColour.x, lightColour.y, lightColour.z, 1.0f );
	lightingBuffer->myLightDirection		= directionalLight->GetDirection();	
	lightingBuffer->myApplyShadows			= directionalLight->GetCastsShadows() ? 1.0f : 0.0f;
	
	// Set the light's view/projection matrix
	XMMATRIX	lightVP						= XMMatrixMultiply( XMLoadFloat4x4(&aLight->GetViewMatrix()), XMLoadFloat4x4(&aLight->GetProjectionMatrix()) );
	lightingBuffer->myLightViewProjection	= XMMatrixTranspose( lightVP );

	// Calculate the inverse view-projection matrix
	XMMATRIX viewProj						= XMMatrixMultiply( XMLoadFloat4x4(&aCamera->GetView()), XMLoadFloat4x4(&aCamera->GetProjection()) );
	XMMATRIX inverseViewProj				= XMMatrixInverse( NULL, viewProj );
	lightingBuffer->myInverseViewProjection = XMMatrixTranspose(inverseViewProj);

	// Release the buffer
	deviceContext->Unmap( myPixelConstantBuffer, 0 );

	// Set the constant buffer for the pixel shader
	deviceContext->PSSetConstantBuffers( 0, 1, &myPixelConstantBuffer );

	return true;
}


// Creates the data layout used to send vertex information to the vertex shader
bool VEDirectionalLightShader::CreateDataLayout( ID3D10Blob* aVertexShader )
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

	// Texture coordinates
	dataLayout[1].SemanticName			= "TEXCOORD";
	dataLayout[1].SemanticIndex			= 0;
	dataLayout[1].Format				= DXGI_FORMAT_R32G32_FLOAT;
	dataLayout[1].InputSlot				= 0;
	dataLayout[1].AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
	dataLayout[1].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
	dataLayout[1].InstanceDataStepRate	= 0;

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
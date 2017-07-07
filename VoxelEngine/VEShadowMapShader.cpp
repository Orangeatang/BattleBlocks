
// --------------------- Includes ---------------------

#include "Stdafx.h"
#include "VEShadowMapShader.h"


#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VERenderManager.h"
#include "VEDirectionalLight.h"
#include "VESpotLight.h"
#include "VEBasicCamera.h"


// ------------------ Namespaces -----------------

using namespace DirectX;


// --------------- Class Functions --------------

// Construction
VEShadowMapShader::VEShadowMapShader() : 
	VEShader( sizeof(MatrixBuffer), 0 )
{
	myVertexShaderFile = L"DeferredShadowMapShader.vs";
	myPixelShaderFile  = L"DeferredShadowMapShader.ps";
}


// Fills the pixel shader constant buffer
bool VEShadowMapShader::PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight )
{
	HRESULT						result;
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	MatrixBuffer*				dataBuffer;

	// Get the direction of the light
	XMFLOAT3 lightDirection;
	switch( aLight->GetType() )
	{
		case LT_Directional :
			{
				VEDirectionalLight* light = dynamic_cast<VEDirectionalLight*>( aLight );
				lightDirection = light->GetDirection();
			}
			break;

		case LT_Spot :
			{
				VESpotLight* light = dynamic_cast<VESpotLight*>( aLight );
				lightDirection = light->GetDirection();
			}
			break;

		default :
			return false;
	}

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
	dataBuffer->myWorld			= XMMatrixTranspose( XMMatrixIdentity() );
	dataBuffer->myView			= XMMatrixTranspose( XMLoadFloat4x4(&aLight->GetViewMatrix()) );
	dataBuffer->myProjection	= XMMatrixTranspose( XMLoadFloat4x4(&aLight->GetProjectionMatrix()) );

	// Release the constant buffer lock
	deviceContext->Unmap( myVertexConstantBuffer, 0 );

	// Set the constant buffer for the vertex shader
	deviceContext->VSSetConstantBuffers( 0, 1, &myVertexConstantBuffer );

	return true;
}


// Creates the data layout used to send vertex information to the vertex shader
bool VEShadowMapShader::CreateDataLayout( ID3D10Blob* aVertexShader )
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

// ------------------------ Includes ------------------------

#include "Stdafx.h"
#include "VESSAOShader.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VEBasicCamera.h"


// ----------------------- Namespaces -----------------------

using namespace DirectX;


// -------------------- Class Functions ---------------------

// Construction
VESSAOShader::VESSAOShader() : VEShader( 0, sizeof(PixelShaderConstants) )
{
	myVertexShaderFile = L"DeferredSSAOShader.vs";
	myPixelShaderFile  = L"DeferredSSAOShader.ps";
}


// Fills in the pixel shader constant buffer
bool VESSAOShader::PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight )
{
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	PixelShaderConstants*		buffer;
	HRESULT						result;

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Grab the lighting constant buffer
	result = deviceContext->Map( myPixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	if( FAILED(result) )
	{
		return false;
	}

	// Fill in the pixel shader constants
	buffer = (PixelShaderConstants*)mappedResource.pData;

	// Calculate the inverse view-projection matrix]
	XMMATRIX inverseProj		= XMMatrixInverse( NULL, XMLoadFloat4x4(&aCamera->GetProjection()) );
	buffer->myView				= XMMatrixTranspose( XMLoadFloat4x4(&aCamera->GetView()) );
	buffer->myInverseProjection = XMMatrixTranspose(inverseProj);
	buffer->myScreenDimensions	= XMFLOAT2( (float)renderInterface->GetScreenWidth(), (float)renderInterface->GetScreenHeight() );

	// Release the buffer
	deviceContext->Unmap( myPixelConstantBuffer, 0 );

	// Set the constant buffer for the pixel shader
	deviceContext->PSSetConstantBuffers( 0, 1, &myPixelConstantBuffer );

	return true;
}


// Creates the data layout used to send vertex information to the vertex shader
bool VESSAOShader::CreateDataLayout( ID3D10Blob* aVertexShader )
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
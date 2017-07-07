
// ------------------------- Includes -------------------------

#include "Stdafx.h"
#include "VEClearGBufferShader.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"


// ---------------------- Class Functions ---------------------

// Construction
VEClearGBufferShader::VEClearGBufferShader() :
	VEShader( 0, 0 )
{
	myVertexShaderFile = L"ClearGBufferShader.vs";
	myPixelShaderFile  = L"ClearGBufferShader.ps";
}


// Creates the data layout used to send information to the vertex shader
bool VEClearGBufferShader::CreateDataLayout( ID3D10Blob* aVertexShader )
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
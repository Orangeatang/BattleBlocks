
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VEQuadRenderer.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VEShader.h"


// --------------------- Namespaces ---------------------

using namespace DirectX;


// ------------------- Class Functions ------------------

// Construction
VEQuadRenderer::VEQuadRenderer() :
	myVertexBuffer( NULL ),
	myIndexBuffer( NULL ),
	myVertexCount( 0 ),
	myIndexCount( 0 )
{
}


// Initialises the vertex and index buffers
bool VEQuadRenderer::Initialise()
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	myVertexCount = 4;
	myIndexCount  = 6;

	if( !CreateVetexBuffer(renderInterface) )
	{
		return false;
	}

	if( !CreateIndexBuffer(renderInterface) )
	{
		return false;
	}

	return true;
}


// Cleans up the memory used by the quad renderer
void VEQuadRenderer::Uninitialise()
{
	if( myVertexBuffer != NULL )
	{
		myVertexBuffer->Release();
		myVertexBuffer = NULL;
	}

	if( myIndexBuffer != NULL )
	{
		myIndexBuffer->Release();
		myIndexBuffer = NULL;
	}
}


// Draws the quad on-screen using the supplied shader
void VEQuadRenderer::DrawQuad( VEShader* aShader )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );
	assert( aShader != NULL );

	// Add the data to the input assembler
	Prepare( renderInterface );

	// Draw the quad
	aShader->DrawIndexed( myIndexCount );
}


// Creates the quad vertex buffer
bool VEQuadRenderer::CreateVetexBuffer( VEDirectXInterface* anInterface )
{
	assert( anInterface != NULL );

	QuadVertex*				vertices;
	D3D11_BUFFER_DESC		bufferDescription;
	D3D11_SUBRESOURCE_DATA	vertexData;
	HRESULT					result;

	// Populate the vertices
	vertices	= new QuadVertex[myVertexCount];
	vertices[0].myVertex	= XMFLOAT3( -1.0f, -1.0f, 1.0f );
	vertices[0].myTexCoords = XMFLOAT2( 0.0f, 1.0f );

	vertices[1].myVertex	= XMFLOAT3( 1.0f, -1.0f, 1.0f );
	vertices[1].myTexCoords = XMFLOAT2( 1.0f, 1.0f );

	vertices[2].myVertex	= XMFLOAT3( -1.0f, 1.0f, 1.0f );
	vertices[2].myTexCoords = XMFLOAT2( 0.0f, 0.0f );

	vertices[3].myVertex	= XMFLOAT3( 1.0f, 1.0f, 1.0f );
	vertices[3].myTexCoords = XMFLOAT2( 1.0f, 0.0f );

	int sz = sizeof(QuadVertex);

	// Fill in the buffer description
	bufferDescription.Usage					= D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth				= sizeof(QuadVertex) * myVertexCount;
	bufferDescription.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags		= 0;
	bufferDescription.MiscFlags				= 0;
	bufferDescription.StructureByteStride	= 0;

	// Set up the buffer resource
	vertexData.pSysMem			= vertices;
	vertexData.SysMemPitch		= 0;
	vertexData.SysMemSlicePitch	= 0;

	// Create the vertex buffer
	result = anInterface->GetDevice()->CreateBuffer( &bufferDescription, &vertexData, &myVertexBuffer );
	
	delete [] vertices;
	vertices = 0;
	
	if( FAILED(result) )
	{
		return true;
	}

	return true;
}


// Creates the quad index buffer
bool VEQuadRenderer::CreateIndexBuffer( VEDirectXInterface* anInterface )
{
	assert( anInterface != NULL );

	D3D11_BUFFER_DESC		bufferDescription;
	D3D11_SUBRESOURCE_DATA	indexData;
	unsigned long*			indices;
	HRESULT					result;

	// Populate the indices
	indices = new unsigned long[myIndexCount];
	indices[0] = 0;
	indices[1] = 3;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	// Fill in the buffer description
	bufferDescription.Usage					= D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth				= sizeof(unsigned long) * myIndexCount;
	bufferDescription.BindFlags				= D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags		= 0;
	bufferDescription.MiscFlags				= 0;
	bufferDescription.StructureByteStride	= 0;

	// Set up the buffer resource
	indexData.pSysMem			= indices;
	indexData.SysMemPitch		= 0;
	indexData.SysMemSlicePitch	= 0;

	// Create the vertex buffer
	result = anInterface->GetDevice()->CreateBuffer( &bufferDescription, &indexData, &myIndexBuffer );

	delete [] indices;
	indices = 0;

	if( FAILED(result) )
	{
		return true;
	}

	return true;
}


void VEQuadRenderer::Prepare( VEDirectXInterface* anInterface )
{
	ID3D11DeviceContext* deviceContext	= anInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	unsigned int	strides[1];
	unsigned int	offsets[1];
	ID3D11Buffer*	bufferPointers[1];

	// Set the stride of the input buffers
	strides[0] = sizeof( QuadVertex );

	// Set the buffer offsets
	offsets[0] = 0;

	// Set the pointers to the buffers
	bufferPointers[0] = myVertexBuffer;

	// Activate the buffers in the input assembler
	deviceContext->IASetVertexBuffers( 0, 1, bufferPointers, strides, offsets );
	deviceContext->IASetIndexBuffer( myIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set the type of primitive that we're rendering
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}
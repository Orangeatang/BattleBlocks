
// ----------------------- Includes -----------------------

#include "Stdafx.h"
#include "VEChunkData.h"

#include "VoxelEngine.h"
#include "VEChunk.h"
#include "VEDirectXInterface.h"
#include "VEVoxel.h"

// ---------------------- Namespaces ----------------------

using namespace DirectX;


// -------------------- Class Functions -------------------

// Construction
VEChunkData::VEChunkData( VEChunk* aChunk ) :
	myChunk( aChunk ),
	myIndexBuffer( NULL ),
	myVertexBuffer( NULL )
{
}


// Initialises the required bits and pieces used for threading
bool VEChunkData::Initialise()
{
	return true;
}


// Cleans up the memory used by the 
void VEChunkData::Uninitialise()
{
	Reset();
}


// Clears the vertex and index buffers
void VEChunkData::Reset()
{
	SetVertexBuffer( NULL );
	SetIndexBuffer( NULL );

	myVertices.clear();
	myIndices.clear();
}


// Adds visible faces in to the vertex & index vectors
void VEChunkData::AddFaces( XMFLOAT3& aPosition, DWORD aVisiblity, XMFLOAT4& aColour )
{
	int currentIndex	= myVertices.size();
	float voxelSize		= myChunk->GetVoxelSize();

	XMFLOAT3 point1( aPosition.x, aPosition.y, aPosition.z );
	XMFLOAT3 point2( aPosition.x + voxelSize, aPosition.y + voxelSize, aPosition.z );
	XMFLOAT3 point3( aPosition.x + voxelSize, aPosition.y, aPosition.z );
	XMFLOAT3 point4( aPosition.x, aPosition.y + voxelSize, aPosition.z );

	XMFLOAT3 point5( aPosition.x, aPosition.y, aPosition.z + voxelSize );
	XMFLOAT3 point6( aPosition.x + voxelSize, aPosition.y + voxelSize, aPosition.z + voxelSize );
	XMFLOAT3 point7( aPosition.x, aPosition.y + voxelSize, aPosition.z + voxelSize );
	XMFLOAT3 point8( aPosition.x + voxelSize, aPosition.y, aPosition.z + voxelSize );

	XMFLOAT3 currentNormal;

	// Front face
	if( aVisiblity & VV_Front)
	{
		currentNormal = XMFLOAT3( 0.0f, 0.0f, -1.0f );

		myVertices.push_back( VoxelVertices(point1, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point2, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point3, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point4, currentNormal, aColour) );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 1 );
		myIndices.push_back( currentIndex + 2 );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 3 );
		myIndices.push_back( currentIndex + 1 );

		currentIndex = myVertices.size();
	}

	// Back face
	if( aVisiblity & VV_Back )
	{
		currentNormal = XMFLOAT3( 0.0f, 0.0f, 1.0f );

		myVertices.push_back( VoxelVertices(point5, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point6, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point7, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point8, currentNormal, aColour) );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 1 );
		myIndices.push_back( currentIndex + 2 );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 3 );
		myIndices.push_back( currentIndex + 1 );

		currentIndex = myVertices.size();
	}

	// Left face
	if( aVisiblity & VV_Left )
	{
		currentNormal = XMFLOAT3( -1.0f, 0.0f, 0.0f );

		myVertices.push_back( VoxelVertices(point5, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point4, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point1, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point7, currentNormal, aColour) );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 1 );
		myIndices.push_back( currentIndex + 2 );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 3 );
		myIndices.push_back( currentIndex + 1 );

		currentIndex = myVertices.size();
	}

	// Right face
	if( aVisiblity & VV_Right )
	{
		currentNormal = XMFLOAT3( 1.0f, 0.0f, 0.0f );

		myVertices.push_back( VoxelVertices(point3, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point6, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point8, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point2, currentNormal, aColour) );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 1 );
		myIndices.push_back( currentIndex + 2 );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 3 );
		myIndices.push_back( currentIndex + 1 );

		currentIndex = myVertices.size();
	}

	// Top face
	if( aVisiblity & VV_Top )
	{
		currentNormal = XMFLOAT3( 0.0f, 1.0f, 0.0f );

		myVertices.push_back( VoxelVertices(point4, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point6, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point2, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point7, currentNormal, aColour) );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 1 );
		myIndices.push_back( currentIndex + 2 );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 3 );
		myIndices.push_back( currentIndex + 1 );

		currentIndex = myVertices.size();
	}

	// Bottom face
	if( aVisiblity & VV_Bottom )
	{
		currentNormal = XMFLOAT3( 0.0f, -1.0f, 0.0f );

		myVertices.push_back( VoxelVertices(point1, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point8, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point5, currentNormal, aColour) );
		myVertices.push_back( VoxelVertices(point3, currentNormal, aColour) );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 1 );
		myIndices.push_back( currentIndex + 2 );

		myIndices.push_back( currentIndex );
		myIndices.push_back( currentIndex + 3 );
		myIndices.push_back( currentIndex + 1 );

		currentIndex = myVertices.size();
	}
}


// Creates a thread that builds the vertex and index buffers
bool VEChunkData::BuildBuffers()
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	D3D11_BUFFER_DESC       bufferDescription;
	D3D11_SUBRESOURCE_DATA  bufferData;
	HRESULT					result;

	// Build the vertex buffer
	ZeroMemory( &bufferDescription, sizeof(D3D11_BUFFER_DESC) );
	bufferDescription.Usage                 = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth             = sizeof(VoxelVertices) * myVertices.size();
	bufferDescription.BindFlags             = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags        = 0;
	bufferDescription.MiscFlags             = 0;
	bufferDescription.StructureByteStride   = 0;

	bufferData.pSysMem          = myVertices.data();
	bufferData.SysMemPitch      = 0;
	bufferData.SysMemSlicePitch = 0;

	result = renderInterface->GetDevice()->CreateBuffer( &bufferDescription, &bufferData, &myVertexBuffer );  
	if( FAILED(result) )
	{
		return false;
	}

	// Build the index buffer
	ZeroMemory( &bufferDescription, sizeof(D3D11_BUFFER_DESC) );
	bufferDescription.Usage                 = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth             = sizeof(unsigned long) * myIndices.size();
	bufferDescription.BindFlags             = D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags        = 0;
	bufferDescription.MiscFlags             = 0;
	bufferDescription.StructureByteStride   = 0;

	bufferData.pSysMem			= myIndices.data();
	bufferData.SysMemPitch		= 0;
	bufferData.SysMemSlicePitch = 0;

	result = renderInterface->GetDevice()->CreateBuffer( &bufferDescription, &bufferData, &myIndexBuffer );
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Sets the vertex buffer data
void VEChunkData::SetVertexBuffer( ID3D11Buffer* aVertexBuffer )
{
	if( myVertexBuffer != NULL )
	{
		myVertexBuffer->Release();
	}

	myVertexBuffer = aVertexBuffer;
}


// Sets the index buffer data
void VEChunkData::SetIndexBuffer( ID3D11Buffer* anIndexBuffer )
{
	if( myIndexBuffer != NULL )
	{
		myIndexBuffer->Release();
	}

	myIndexBuffer = anIndexBuffer;
}
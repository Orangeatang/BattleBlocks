
// ----------------------- Includes -----------------------

#include "Stdafx.h"
#include "VESphere.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"


// ----------------------- Namespaces ---------------------

using namespace DirectX;


// -------------------- Class Functions -------------------

// Construction
VESphere::VESphere( int aRefinementLevel /* = 3 */ ) :
	myRefinementLevel( aRefinementLevel ),
	myVertexBuffer( NULL ),
	myIndexBuffer( NULL )
{
}


// Deconstruction
VESphere::~VESphere()
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


// Builds the icosahedron mesh (http://1.bp.blogspot.com/_-FeuT9Vh6rk/Sj1WHbcQwxI/AAAAAAAAABw/xaFDct6AyOI/s400/icopoints.png)
bool VESphere::CreateMesh()
{
	// Create the 12 vertices for the icosahedron
	myVertices.clear();
	CreateInitialVertices();

	// Now create the initial 20 faces of the icosahedron
	std::vector<FaceIndices> faces;
	CreateInitialFaces( faces );

	// Subdivide the triangles
	myMiddlePointIndexCache.clear();
	RefineFaces( faces );

	// Add the faces to the index buffer
	myIndices.clear();
	for( unsigned int i = 0; i < faces.size(); i++ )
	{
		myIndices.push_back( faces[i].myVertex1 );
		myIndices.push_back( faces[i].myVertex2 );
		myIndices.push_back( faces[i].myVertex3 );
	}

	// Create the vertex and index buffers
	if( !CreateVertexBuffer() )
	{
		return false;
	}

	if( !CreateIndexBuffer() )
	{
		return false;
	}

	return true;
}


// Loads the vertices and indices in to the input assembler
void VESphere::Prepare( /* add the instance buffer as a parameter here */ )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	unsigned int	strides[1];
	unsigned int	offsets[1];
	ID3D11Buffer*	bufferPointers[1];

	// Set the stride of the input buffers
	strides[0] = sizeof( XMFLOAT3 );

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


// Creates the initial 12 vertices used in the icosahedron
void VESphere::CreateInitialVertices()
{
	// Create the initial 12 vertices of the icosahedron
	float t = (float)(1.0 + sqrt(5.0)) / 2.0f;

	AddVertex( XMFLOAT3(-1, t, 0) );
	AddVertex( XMFLOAT3(1, t, 0) );
	AddVertex( XMFLOAT3(-1, -t, 0) );
	AddVertex( XMFLOAT3(1, -t, 0) );

	AddVertex( XMFLOAT3(0, -1, t) );
	AddVertex( XMFLOAT3(0, 1, t) );
	AddVertex( XMFLOAT3(0, -1, -t) );
	AddVertex( XMFLOAT3(0, 1, -t) );

	AddVertex( XMFLOAT3(t, 0, -1) );
	AddVertex( XMFLOAT3(t, 0, 1) );
	AddVertex( XMFLOAT3(-t, 0, -1) );
	AddVertex( XMFLOAT3(-t, 0, 1) );
}


// Creates the initial faces of the icosahedron
void VESphere::CreateInitialFaces( std::vector<FaceIndices>& someFaces )
{
	assert( myVertices.size() == 12 );
	someFaces.clear();

	// The five faces around point 0
	someFaces.push_back( FaceIndices(0, 5, 11) );
	someFaces.push_back( FaceIndices(0, 1, 5) );
	someFaces.push_back( FaceIndices(0, 7, 1) );
	someFaces.push_back( FaceIndices(0, 10, 7) );
	someFaces.push_back( FaceIndices(0, 11, 10) );

	// The five faces adjacent to point 0
	someFaces.push_back( FaceIndices(1, 9, 5) );
	someFaces.push_back( FaceIndices(5, 4, 11) );
	someFaces.push_back( FaceIndices(11, 2, 10) );
	someFaces.push_back( FaceIndices(10, 6, 7) );
	someFaces.push_back( FaceIndices(7, 8, 1) );

	// The five faces around point 3
	someFaces.push_back( FaceIndices(3, 4, 9) );
	someFaces.push_back( FaceIndices(3, 2, 4) );
	someFaces.push_back( FaceIndices(3, 6, 2) );
	someFaces.push_back( FaceIndices(3, 8, 6) );
	someFaces.push_back( FaceIndices(3, 9, 8) );

	// And the five faces adjacent to point 3
	someFaces.push_back( FaceIndices(4, 5, 9) );
	someFaces.push_back( FaceIndices(2, 11, 4) );
	someFaces.push_back( FaceIndices(6, 10, 2) );
	someFaces.push_back( FaceIndices(8, 7, 6) );
	someFaces.push_back( FaceIndices(9, 1, 8) );
}


// Subdivides the triangles in the sphere mesh
void VESphere::RefineFaces( std::vector<FaceIndices>& someFaces )
{
	for( int i = 0; i < myRefinementLevel; i++ )
	{
		// Loop through the current faces, and subdivide each one in to two separate triangles
		std::vector<FaceIndices> newFaces;
		for( unsigned int j = 0; j < someFaces.size(); j++ )
		{
			int a = GetMiddlePoint( someFaces[j].myVertex1, someFaces[j].myVertex2 );
			int b = GetMiddlePoint( someFaces[j].myVertex2, someFaces[j].myVertex3 );
			int c = GetMiddlePoint( someFaces[j].myVertex3, someFaces[j].myVertex1 );

			newFaces.push_back( FaceIndices(someFaces[j].myVertex1, c, a) );
			newFaces.push_back( FaceIndices(someFaces[j].myVertex2, a, b) );
			newFaces.push_back( FaceIndices(someFaces[j].myVertex3, b, c) );
			newFaces.push_back( FaceIndices(a, c, b) );
		}

		// Copy the new face data in to the old face data vector
		someFaces.clear();
		someFaces.reserve( newFaces.size() );
		for( unsigned int j = 0; j < newFaces.size(); j++ )
		{
			someFaces.push_back( newFaces[j] );
		}
	}
}


// Adds a vertex to the mesh, fixing the position so it's a unit sphere. The index of
// the new vertex is returned
int VESphere::AddVertex( const DirectX::XMFLOAT3& aVertex )
{
	float length = (float)sqrt( aVertex.x * aVertex.x + aVertex.y * aVertex.y + aVertex.z * aVertex.z );
	myVertices.push_back( XMFLOAT3(aVertex.x / length, aVertex.y / length, aVertex.z / length) );

	return (myVertices.size() - 1);
}


// Returns the index of the vertex in between the supplied vertices (the new vertex is added automagically)
int VESphere::GetMiddlePoint( int aVertex1, int aVertex2 )
{
	// Calculate an index key, to see if we already have the point cached
	bool	firstPointSmaller	= aVertex1 < aVertex2;
	INT64	smallerIndex		= firstPointSmaller ? aVertex1 : aVertex2;
	INT64	largerIndex			= firstPointSmaller ? aVertex2 : aVertex1;
	INT64	key					= (smallerIndex << 32) + largerIndex;

	// Check the cache
	if( myMiddlePointIndexCache.find(key) != myMiddlePointIndexCache.end() )
	{
		return myMiddlePointIndexCache[key];
	}

	// Otherwise generate a new point
	XMFLOAT3 vertex1		= myVertices[aVertex1];
	XMFLOAT3 vertex2		= myVertices[aVertex2];
	XMFLOAT3 middleVertex	= XMFLOAT3( (vertex1.x + vertex2.x) / 2.0f, (vertex1.y + vertex2.y) / 2.0f, (vertex1.z + vertex2.z) / 2.0f );

	// Add the new point
	int ret = AddVertex( middleVertex );

	// Store the vertex in the cache
	myMiddlePointIndexCache.insert( std::pair<INT64,int>(key, ret) );
	
	return ret;
}


// Creates the icosphere's vertex buffer
bool VESphere::CreateVertexBuffer()
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	D3D11_BUFFER_DESC       bufferDescription;
	D3D11_SUBRESOURCE_DATA  bufferData;
	HRESULT					result;

	// Build the vertex buffer
	ZeroMemory( &bufferDescription, sizeof(D3D11_BUFFER_DESC) );
	bufferDescription.Usage                 = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth             = sizeof(XMFLOAT3) * myVertices.size();
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

	return true;
}


// Creates the icosphere's index buffer
bool VESphere::CreateIndexBuffer()
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	D3D11_BUFFER_DESC       bufferDescription;
	D3D11_SUBRESOURCE_DATA  bufferData;
	HRESULT					result;

	// Build the index buffer
	ZeroMemory( &bufferDescription, sizeof(D3D11_BUFFER_DESC) );
	bufferDescription.Usage                 = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth             = sizeof(int) * myIndices.size();
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
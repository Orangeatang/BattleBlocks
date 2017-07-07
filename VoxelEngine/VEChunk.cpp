
// ------------------------ Includes ------------------------

#include "Stdafx.h"
#include "VEChunk.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VEVoxel.h"
#include "VEChunkData.h"
#include "VEThreadManager.h"
#include "VEChunkManager.h"

#include "noiseutils.h"


// ----------------------- Namespaces -----------------------

using namespace DirectX;


// --------------------- Global Functions -------------------

// A thread function that builds the chunk's data
UINT VEChunk::BuildDataThread( LPVOID someData )
{
	VEChunk* chunk = reinterpret_cast<VEChunk*>( someData );
	if( chunk == NULL )
	{
		ExitThread( -1 );
	}

	VEChunkData* renderData = chunk->GetRenderData();
	if( renderData == NULL )
	{
		ExitThread( -1 );
	}

	XMFLOAT3	chunkPosition	= chunk->GetPosition();
	int			chunkDimensions = chunk->GetDimensions();
	float		voxelSize		= chunk->GetVoxelSize();
	float		halfWidth		= (float)(chunkDimensions / 2) * voxelSize;
	XMFLOAT3	voxelPosition( chunkPosition.x, chunkPosition.y, chunkPosition.z );
	
	// For the moment all voxels default to the grass colour
	XMFLOAT4 grassColour = XMFLOAT4(0.0f, 0.36f, 0.04f, 1.0f);

	// Generate the vertex & index data
	EnterCriticalSection( chunk->GetCriticalSection() );
	renderData->Reset();
	for( int y = 0; y < chunkDimensions; y++ )
	{
		for( int x = 0; x < chunkDimensions; x++ )
		{
			for( int z = 0; z < chunkDimensions; z++ )
			{
				VEVoxel* currentVoxel = chunk->GetVoxel( x, y, z );
				chunk->CalculateVoxelVisibility( x, y, z );

				if( currentVoxel->GetVisibility() == VV_None )
				{
					voxelPosition.z	+= voxelSize;
					continue;
				}

				// If the voxel is visible, add the geometry to the chunk
				chunk->myRenderData->AddFaces( voxelPosition, currentVoxel->GetVisibility(), grassColour );

				voxelPosition.z += voxelSize;
			}

			voxelPosition.x += voxelSize;
			voxelPosition.z = chunkPosition.z;
		}

		voxelPosition.y += voxelSize;
		voxelPosition.x = chunkPosition.x;
	}

	// Build the vertex and index buffers
	if( !renderData->BuildBuffers() )
	{
		LeaveCriticalSection( chunk->GetCriticalSection() );
		ExitThread( -1 );
	}

	chunk->SetEnabled( true );
	LeaveCriticalSection( chunk->GetCriticalSection() );

	ExitThread( 0 );
}


// --------------------- Class Functions --------------------

// Construction
VEChunk::VEChunk( int anId, int aChunkDimensions, int aGridX, int aGridZ ) :
	myGridX( aGridX ),
	myGridZ( aGridZ ),
	myVoxels( NULL ),
	myChunkDimensions( aChunkDimensions ),
	myIsDirty( false ),
	myEnabled( false ),
	myVoxelSize( 1.0f ),
	myId( anId ),
	myRenderData( NULL ),
	myMaxHeight( 20 )
{
	myPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );
}


// Creates the voxel array and builds the initial instance buffer
bool VEChunk::Initialise( XMFLOAT3 aChunkPosition )
{
	myPosition = aChunkPosition;
	
	// Initialise the voxel array
	myVoxels = new VEVoxel**[myChunkDimensions];
	for( int x = 0; x < myChunkDimensions; x++ )
	{
		myVoxels[x] = new VEVoxel*[myChunkDimensions];
		for( int y = 0; y < myChunkDimensions; y++ )
		{
			myVoxels[x][y] = new VEVoxel[myChunkDimensions];
			if( myGridX == 0 )
			{
				myVoxels[x][y]->SetType( VT_Water );
			}
		}
	}

	myRenderData = new VEChunkData( this );
	if( !myRenderData->Initialise() )
	{
		return false;
	}

	InitializeCriticalSection( &myCriticalSection );

	// Signal that we need to build the chunk vertex & index buffers, but don't actually build...
	// other classes may want to alter the structure of the chunk before this happens
	myIsDirty = true;

	return true;
}


// Cleans up the memory used by the chunk
void VEChunk::Uninitialise()
{
	if( myVoxels != NULL )
	{
		for( int x = 0; x < myChunkDimensions; x++ )
		{
			for( int y = 0; y < myChunkDimensions; y++ )
			{
				delete [] myVoxels[x][y];
			}

			delete [] myVoxels[x];
		}

		delete [] myVoxels;
		myVoxels = NULL;
	}

	DeleteCriticalSection( &myCriticalSection );

	if( myRenderData != NULL )
	{
		myRenderData->Uninitialise();
		
		delete myRenderData;
		myRenderData = NULL;
	}
}


// Applies a particular style to the chunk
void VEChunk::ApplyStyle( ChunkStyle aStyle )
{
	switch( aStyle )
	{
		case CS_Box :
			GenerateBox();
			break;

		case CS_Sphere :
			GenerateSphere();
			break;

		case CS_Pyramid :
			GeneratePyramid();
			break;

		case CS_Empty :
			GenerateEmpty();
			break;
			
		default :
			break;
	}

	myIsDirty = true;
}


// Applies a height map to the chunk
void VEChunk::ApplyHeightMap( noise::utils::NoiseMap* aHeightMap )
{
	assert( aHeightMap != NULL );

	GenerateEmpty();

	for( int x = 0; x < myChunkDimensions; x++ )
	{
		for( int z = 0; z < myChunkDimensions; z++ )
		{
			// Get the current height value
			float height = aHeightMap->GetValue(x, z);

			// Map the height value to the 0.0f - 1.0f range
			height = (height * 0.5f) + 0.5f;

			// The height value from the noise map should be treated like a percentage of the
			// maximum terrain height for the current chunk
			int terrainHeight = (int)( height * myMaxHeight );

			if( terrainHeight < 1.0f )
			{
				myVoxels[x][0][z].SetEnabled( true );
			}
			else
			{
				for( int y = 0; y < terrainHeight; y++ )
				{
					myVoxels[x][y][z].SetEnabled( true );
				}
			}
		}
	}

	myIsDirty = true;
}


// Returns the voxel at the supplied coordinates
VEVoxel* VEChunk::GetVoxel( int anX, int aY, int aZ )
{
	int chunkLimit = myChunkDimensions - 1;

	if( anX < 0 || anX > chunkLimit || aY < 0 || aY > chunkLimit || aZ < 0 || aZ > chunkLimit )
	{
		return NULL;
	}

	return &myVoxels[anX][aY][aZ];
}


// Builds the vertex & index buffers used for rendering the chunk
void VEChunk::Rebuild()
{
	assert( myRenderData != NULL );

	myEnabled = false;
	myIsDirty = false;

	VEThreadManager* threadManager = VoxelEngine::GetInstance()->GetThreadManager();
	assert( threadManager != NULL );

	threadManager->SpawnThread( (LPTHREAD_START_ROUTINE)VEChunk::BuildDataThread, this );
}


// Prepares the chunk for rendering, loading the vertex & index buffers in to the
// input assembler
void VEChunk::Prepare()
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	unsigned int	strides[1];
	unsigned int	offsets[1];
	ID3D11Buffer*	bufferPointers[1];

	// Set the stride of the input buffers
	strides[0] = sizeof( VoxelVertices );

	// Set the buffer offsets
	offsets[0] = 0;

	// Set the pointers to the buffers
	bufferPointers[0] = myRenderData->GetVertexBuffer();

	// Activate the buffers in the input assembler
	deviceContext->IASetVertexBuffers( 0, 1, bufferPointers, strides, offsets );
	deviceContext->IASetIndexBuffer( myRenderData->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0 );

	// Set the type of primitive that we're rendering
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}


// Returns the number of indices in the chunk's render data
int VEChunk::GetIndexCount()
{
	if( myRenderData == NULL )
	{
		return 0;
	}

	return myRenderData->GetIndexCount();
}


// Sets the visibility of a voxel based on surrounding voxels & chunks
void VEChunk::CalculateVoxelVisibility( int anX, int aY, int aZ )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEVoxel* voxel = GetVoxel( anX, aY, aZ );
	if( !voxel->GetEnabled() )
	{
		voxel->SetVisibility( VV_None );
		return;
	}

	DWORD visibility	= VV_All;
	int	  chunkBounds	= myChunkDimensions - 1;

	// ------ Left & right face checks ------

	if( anX	== 0 )
	{
		if( CheckAdjacentChunk(chunkBounds, aY, aZ, myGridX - 1, myGridZ) )
		{
			visibility &= ~VV_Left;
		}
	}

	if( anX < chunkBounds )
	{
		if( myVoxels[anX + 1][aY][aZ].GetEnabled() )
		{
			visibility &= ~VV_Right;
		}
	}
	else
	{
		if( CheckAdjacentChunk(0, aY, aZ, myGridX + 1, myGridZ) )
		{
			visibility &= ~VV_Right;
		}
	}

	if( anX > 0 )
	{
		if( myVoxels[anX - 1][aY][aZ].GetEnabled() )
		{
			visibility &= ~VV_Left;
		}
	}

	// ------ Top & bottom face checks ------

	// Is the top face blocked
	if( aY < chunkBounds )
	{
		if( myVoxels[anX][aY + 1][aZ].GetEnabled() )
		{
			visibility &= ~VV_Top;
		}
	}

	// Is the bottom face blocked
	if( aY > 0 )
	{
		if( myVoxels[anX][aY -1][aZ].GetEnabled() )
		{
			visibility &= ~VV_Bottom;
		}
	}

	// ------ Front & back face checks ------
	
	if( aZ == 0 )
	{
		if( CheckAdjacentChunk(anX, aY, chunkBounds, myGridX, myGridZ - 1) )
		{
			visibility &= ~VV_Front;
		}
	}
	
	if( aZ < chunkBounds )
	{
		if( myVoxels[anX][aY][aZ + 1].GetEnabled() )
		{
			visibility &= ~VV_Back;
		}
	}
	else 
	{
		if( CheckAdjacentChunk(anX, aY, 0, myGridX, myGridZ + 1) )
		{
			visibility &= ~VV_Back;
		}
	}

	if( aZ > 0 )
	{
		if( myVoxels[anX][aY][aZ - 1].GetEnabled() )
		{
			visibility &= ~VV_Front;
		}
	}

	voxel->SetVisibility( visibility );
}


// Checks the visibility of the voxel at the supplied co-ordinates in an adjacent chunk
bool VEChunk::CheckAdjacentChunk( int anX, int aY, int aZ, int aChunkX, int aChunkZ )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEChunk* nextChunk = chunkManager->GetChunk( aChunkX, aChunkZ );
	if( nextChunk != NULL )
	{
		VEVoxel* adjacentVoxel = nextChunk->GetVoxel( anX, aY, aZ );
		if( adjacentVoxel != NULL )
		{
			return adjacentVoxel->GetEnabled();
		}
	}

	return false;
}


// Enables all voxels in the chunk
void VEChunk::GenerateBox()
{
	for( int x = 0; x < myChunkDimensions; x++ )
	{
		for( int y = 0; y < myChunkDimensions; y++ )
		{
			for( int z = 0; z < myChunkDimensions; z++ )
			{
				myVoxels[x][y][z].SetEnabled( true );
			}
		}
	}
}


// Enables voxels making the chunk look like a sphere
void VEChunk::GenerateSphere()
{
	for( int x = 0; x < myChunkDimensions; x++ )
	{
		for( int y = 0; y < myChunkDimensions; y++ )
		{
			for( int z = 0; z < myChunkDimensions; z++ )
			{
				float halfChunkSize = (float)myChunkDimensions / 2.0f;
				float x2		= (x - halfChunkSize) * (x - halfChunkSize);
				float y2		= (y - halfChunkSize) * (y - halfChunkSize);
				float z2		= (z - halfChunkSize) * (z - halfChunkSize);
				float factor	= sqrt( x2 + y2 + z2 );

				if( factor <= halfChunkSize )
				{
					myVoxels[x][y][z].SetEnabled( true );
				}
				else
				{
					myVoxels[x][y][z].SetEnabled( false );
				}
			}
		}
	}
}


// Makes the chunk resemble a pyramid
void VEChunk::GeneratePyramid()
{
	int xLimit = 0;
	int zLimit = 0;

	// Disable all voxels
	GenerateEmpty();

	// Enable voxels in a pyramid shape
	for( int y = 0; y < myChunkDimensions; y++ )
	{
		for( int x = xLimit; x < myChunkDimensions - xLimit; x++ )
		{
			for( int z = zLimit; z < myChunkDimensions - zLimit; z++ )
			{
				myVoxels[x][y][z].SetEnabled( true );
			}
		}

		xLimit++;
		zLimit++;
	}
}


// Disables all voxels
void VEChunk::GenerateEmpty()
{
	for( int x = 0; x < myChunkDimensions; x++ )
	{
		for( int y = 0; y < myChunkDimensions; y++ )
		{
			for( int z = 0; z < myChunkDimensions; z++ )
			{
				myVoxels[x][y][z].SetEnabled( false );
			}
		}
	}
}
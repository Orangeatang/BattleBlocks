
// ------------------------- Includes -------------------------

#include "Stdafx.h"
#include "VEChunkManager.h"

#include "VEChunk.h"


// ------------------------- Namespaces -----------------------

using namespace DirectX;


// ---------------------- Class Functions ---------------------

// Construction
VEChunkManager::VEChunkManager() :
	myNextChunkId( 0 ),
	myChunkDimensions( 0 ),
	myGridWidth( 0 ),
	myGridDepth( 0 )
{
}


// Creates a x * y voxel chunks, that can be accessed like a 2D array
bool VEChunkManager::CreateGrid( int aWidth /* = 5 */, int aDepth /* = 5 */, int aChunkDimensions /* = 64 */, const DirectX::XMFLOAT3& aStartPosition /* = XMFLOAT3(0.0f, 0.0f, 0.0f) */ )
{
	if( aWidth <= 0 || aDepth <= 0 || aChunkDimensions <= 0 )
	{
		return false;
	}

	myChunkDimensions	= aChunkDimensions;
	myGridWidth			= aWidth;
	myGridDepth			= aDepth;

	if( myChunks.size() > 0 )
	{
		Uninitialise();
	}

	// Create the grid of chunks
	XMFLOAT3 currentPosition = aStartPosition;
	for( int z = 0; z < myGridDepth; z++ )
	{
		for( int x = 0; x < myGridWidth; x++ )
		{
			VEChunk* newChunk = AddChunk( currentPosition, x, z );
			currentPosition.x += myChunkDimensions;
		}

		currentPosition.z += myChunkDimensions;
		currentPosition.x = 0.0f;
	}

	return true;
}


// Cleans the memory used by the chunks
void VEChunkManager::Uninitialise()
{
	for( unsigned int i = 0; i < myChunks.size(); i++ )
	{
		if( myChunks[i] != NULL )
		{
			myChunks[i]->Uninitialise();

			delete myChunks[i];
			myChunks[i] = NULL;
		}
	}

	myChunks.clear();
}


// Updates the chunks that need to be rebuilt
void VEChunkManager::Update( float anElapsedTime )
{
	for( unsigned int i = 0; i < myChunks.size(); i++ )
	{
		if( myChunks[i]->GetIsDirty() )
		{
			myChunks[i]->Rebuild();
		}
	}
}


// Returns the chunk that is active at the supplied position. The position is converted in to 'chuck-grid-space', and the appropriate chunk is 
// returned. This works on the principal that all chunks have a positive x/z coordinates. 
VEChunk* VEChunkManager::GetChunk( const DirectX::XMFLOAT3& aPosition )
{
	// TODO - offset by voxel size
	int chunkPositionX = (int)aPosition.x / myChunkDimensions;
	int chunkPositionZ = (int)aPosition.z / myChunkDimensions;

	return GetChunk( chunkPositionX, chunkPositionZ );
}


// Calculates the offset (in voxels) of the supplied position, given an active chunk
void VEChunkManager::CalculateVoxelOffset( DirectX::XMINT3& aChunkOffset, const DirectX::XMFLOAT3& aCurrentPosition, const VEChunk* aChunk )
{
	const XMFLOAT3	chunkPosition	= aChunk->GetPosition();
	const float		voxelSize		= aChunk->GetVoxelSize();

	aChunkOffset = XMINT3( (int)((aCurrentPosition.x - chunkPosition.x) / voxelSize), 
		(int)((aCurrentPosition.y - chunkPosition.y) / voxelSize), 
		(int)((aCurrentPosition.z - chunkPosition.z) / voxelSize) );
}


// Returns a pointer to the chunk at the calculated index
VEChunk* VEChunkManager::GetChunk( int anX, int aZ )
{
	if( anX < 0 || aZ < 0 || anX >= myGridWidth || aZ >= myGridDepth )
	{
		return NULL;
	}

	unsigned int index = (myGridWidth * aZ) + anX;
	if( index >= myChunks.size() )
	{
		return NULL;
	}

	return myChunks[index];
}


// Adds a chunk to the engine
VEChunk* VEChunkManager::AddChunk( const XMFLOAT3& aPosition, int aGridX, int aGridZ )
{
	// Create the new chunk
	VEChunk* newChunk = new VEChunk( myNextChunkId++, myChunkDimensions, aGridX, aGridZ );
	if( newChunk == NULL )
	{
		return NULL;
	}

	// Initialise the chunk
	if( !newChunk->Initialise(aPosition) )
	{
		delete newChunk;
		newChunk = NULL;

		return NULL;
	}

	myChunks.push_back( newChunk );
	return newChunk;
}


// Removes a chunk from the engine
bool VEChunkManager::RemoveChunk( int aChunkId )
{
	for( unsigned int i = 0; i < myChunks.size(); i++ )
	{
		if( myChunks[i]->GetId() == aChunkId )
		{
			delete myChunks[i];
			myChunks[i] = NULL;

			myChunks.erase( myChunks.begin() + i );
			return true;
		}
	}

	return false;
}
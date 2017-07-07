#ifndef VE_CHUNK_MANAGER_H
#define VE_CHUNK_MANAGER_H


// -------------------------- Includes -----------------------

#include "VETypes.h"


// ------------------- Forward Declarations ------------------

class VEChunk;


// ------------------------- Classes -------------------------

// The chunk manager maintains all of the active chunks in the engine, providing methods for adding
// new chunks and removing old ones
class VEChunkManager
{
	public :

		// ---------- Public Functions ----------

		// Construction
		VEChunkManager();

		// Creates x * y voxel chunks, that can be accessed like a 2D array
		bool							CreateGrid( int aWidth = 5, int aDepth = 5, int aChunkDimensions = 64, const DirectX::XMFLOAT3& aStartPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) );	

		// Cleans the memory used by the chunks
		void							Uninitialise();

		// Updates the chunks that need to be rebuilt
		void							Update( float anElapsedTime );

		// Returns the chunk that is active at the supplied position. The position is converted in to 'chuck-grid-space', and the appropriate chunk is 
		// returned. This works on the principal that all chunks have a positive x/z coordinates. 
		VEChunk*						GetChunk( const DirectX::XMFLOAT3& aPosition );

		// Calculates the offset (in voxels) of the supplied position, given an active chunk
		void							CalculateVoxelOffset( DirectX::XMINT3& aChunkOffset, const DirectX::XMFLOAT3& aCurrentPosition, const VEChunk* aChunk );


		// ------------- Accessors --------------

		VEChunk*						GetChunk( int anX, int aZ );
		
		const std::vector<VEChunk*>&	GetChunks()				{ return myChunks; }

		int								GetChunkDimensions()	{ return myChunkDimensions; }


	private :

		// ---------- Private Functions ---------
		
		// Adds a chunk to the manager
		VEChunk*						AddChunk( const DirectX::XMFLOAT3& aPosition, int aGridX, int aGridZ );
			
		// Removes a chunk from the manager
		bool							RemoveChunk( int aChunkId );


		// ---------- Private Variables ---------

		std::vector<VEChunk*>	myChunks;
		int						myNextChunkId;

		int						myChunkDimensions;
		int						myGridWidth;
		int						myGridDepth;
};


#endif // !VE_CHUNK_MANAGER_H
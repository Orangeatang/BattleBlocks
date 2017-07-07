#ifndef VE_CHUNK_DX_H
#define VE_CHUNK_DX_H


// ------------------------ Includes ------------------------

#include "VETypes.h"


// ------------------ Forward Declarations ------------------

class	VEVoxelData;
class	VEVoxel;
class	VEChunkData;

namespace noise
{
	namespace utils
	{
		class NoiseMap;
	}
}


// ------------------------ Classes -------------------------

// A chunk groups a number of voxels together in a three dimensional array. It also generates the vertex
// and index buffers used for drawing all of the voxels in the chunk. Note that non-visible faces are 
// culled from the rendering when the data is generated
class VEChunk
{
	public :

		// ------- Public Functions -------

		// Construction
		VEChunk( int anId, int aChunkDimensions, int aGridX, int aGridZ );

		// Creates the voxel array and builds the initial instance buffer. The position is at the center of the chunk, voxels
		// are drawn around it
		bool				Initialise( DirectX::XMFLOAT3 aChunkPosition );

		// Cleans up the memory used by the chunk
		void				Uninitialise();

		// Applies a particular style to the chunk
		void				ApplyStyle( ChunkStyle aStyle );
		
		// Applies a height map to the chunk
		void				ApplyHeightMap( noise::utils::NoiseMap* aHeightMap );

		// Returns the voxel at the supplied coordinates
		VEVoxel*			GetVoxel( int anX, int aY, int aZ );

		// Converts the supplied world space coordinates to voxel space coordinates
		void				GetVoxelSpaceCoordinates( const DirectX::XMFLOAT3& aWorldPosition, DirectX::XMFLOAT3& aVoxelPosition );

		// Builds the vertex & index buffers used for rendering the chunk
		void				Rebuild();
		
		// Prepares the chunk for rendering, loading the vertex, index & instance buffers in to the
		// input assembler
		void				Prepare();


		// ---------- Accessors -----------

		int							GetId()												{ return myId; }

		bool						GetEnabled()										{ return myEnabled; }
		void						SetEnabled( bool anIsReady )						{ myEnabled = anIsReady; }

		bool						GetIsDirty()										{ return myIsDirty; }

		int							GetDimensions() const								{ return myChunkDimensions; }

		float						GetVoxelSize() const								{ return myVoxelSize; }

		const DirectX::XMFLOAT3&	GetPosition() const									{ return myPosition; }

		VEVoxel***					GetVoxels() 										{ return myVoxels; }

		void						SetPosition( const DirectX::XMFLOAT3& aPosition )	{ myIsDirty = true; myPosition = aPosition; }
	
		CRITICAL_SECTION*			GetCriticalSection()								{ return &myCriticalSection; }

		VEChunkData*				GetRenderData()										{ return myRenderData; }

		int							GetMaxHeight()										{ return myMaxHeight; }
		void						SetMaxHeight( int aMaxHeight)						{ myMaxHeight = aMaxHeight; }

		int							GetIndexCount();


	private :

		// ------- Private Functions ------

		// Sets the visibility of a voxel based on surrounding voxels & chunks
		void						CalculateVoxelVisibility( int anX, int aY, int aZ );

		// Checks the visibility of the voxel at the supplied co-ordinates in an adjacent chunk
		bool						CheckAdjacentChunk( int anX, int aY, int aZ, int aChunkX, int aChunkZ );

		// Enables all voxels in the chunk
		void						GenerateBox();

		// Makes the chunk resemble a sphere
		void						GenerateSphere();		
		
		// Makes the chunk resemble a pyramid
		void						GeneratePyramid();

		// Enables voxels based on height map data
		void						GenerateHeightmap();

		// Disables all voxels
		void						GenerateEmpty();

		// A thread function that builds the chunk's data
		static UINT					BuildDataThread( LPVOID someData );


		// ------- Private Variables ------
		
		DirectX::XMFLOAT3			myPosition;

		int							myGridX;
		int							myGridZ;

		int							myMaxHeight;

		VEVoxel***					myVoxels;
		VEChunkData*				myRenderData;
		const int					myChunkDimensions;
		float						myVoxelSize;

		CRITICAL_SECTION			myCriticalSection;

		bool						myIsDirty;		
		bool						myEnabled;

		int							myId;
};


#endif // !VE_CHUNK_DX_H
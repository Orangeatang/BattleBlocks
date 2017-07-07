#ifndef VE_CHUNK_DATA_H
#define VE_CHUNK_DATA_H


// ----------------------- Includes -----------------------

#include "VETypes.h"


// ------------------ Forward Declarations ----------------

class VEChunk;


// ----------------------- Classes ------------------------

// The chunk data class maintains the vertex and index buffer data for a single chunk
class VEChunkData
{
	public :

		// ------- Public Functions -------

		// Construction
		VEChunkData( VEChunk* aChunk );

		// Initialises the required bits and pieces used for threading
		bool	Initialise();

		// Cleans up the memory used by the 
		void	Uninitialise();

		// Clears the vertex and index buffers
		void	Reset();

		// Adds voxel vertices and indices based on the visible faces
		void	AddFaces( DirectX::XMFLOAT3& aPosition, DWORD aVisability, DirectX::XMFLOAT4& aColour );

		// Creates a thread that builds the vertex and index buffers
		bool	BuildBuffers();


		// ---------- Accessors ----------

		ID3D11Buffer*				GetVertexBuffer()									{ return myVertexBuffer; }
		void						SetVertexBuffer( ID3D11Buffer* aVertexBuffer );

		ID3D11Buffer*				GetIndexBuffer()									{ return myIndexBuffer; }
		void						SetIndexBuffer( ID3D11Buffer* anIndexBuffer );

		std::vector<VoxelVertices>&	GetVertices()										{ return myVertices; }
		std::vector<unsigned long>& GetIndices()										{ return myIndices; }

		int							GetIndexCount()										{ return myIndices.size(); }


	private :

		// ------- Private Functions ------

		// ------- Private Variables ------

		ID3D11Buffer*				myVertexBuffer;
		std::vector<VoxelVertices>	myVertices;

		ID3D11Buffer*				myIndexBuffer;
		std::vector<unsigned long>	myIndices;

		VEChunk*					myChunk;

		
};


#endif // !VE_CHUNK_DATA_H
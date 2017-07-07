#ifndef VE_QUAD_RENDERER_H
#define VE_QUAD_RENDERER_H


// ---------------- Forward Declarations ---------------

class VEDirectXInterface;
class VEShader;


// ---------------------- Classes ----------------------

// Used to draw full screen quads
class VEQuadRenderer
{
	public :

		// --------- Public Functions ---------

		// Construction
		VEQuadRenderer();

		// Initialises the vertex and index buffers
		bool Initialise();

		// Cleans up the memory used by the quad renderer
		void Uninitialise();

		// Draws the quad on-screen using the supplied shader
		void DrawQuad( VEShader* aShader );


	private :

		// --------- Private Functions --------

		// Creates the quad vertex buffer
		bool CreateVetexBuffer( VEDirectXInterface* anInterface );

		// Creates the quad index buffer
		bool CreateIndexBuffer( VEDirectXInterface* anInterface );

		// Adds the vertices and indices to the input assembler
		void Prepare( VEDirectXInterface* anInterface );


		// --------- Private Structures -------

		// A very simple structure to hold the vertices for a quad
		struct QuadVertex
		{
			DirectX::XMFLOAT3 myVertex;
			DirectX::XMFLOAT2 myTexCoords;
		};


		// --------- Private Variables --------

		ID3D11Buffer*	myVertexBuffer;
		ID3D11Buffer*	myIndexBuffer;

		int				myVertexCount;
		int				myIndexCount;
};


#endif
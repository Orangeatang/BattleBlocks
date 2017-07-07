#ifndef VE_SPHERE_H
#define VE_SPHERE_H


// ----------------------- Classes -----------------------

// The sphere class programmatically generates an icosphere mesh - a unit sphere where the vertices are evenly
// distributed around the surface of the mesh. It works by creating a very basic mesh, and then subdividing the
// triangles based on the refinement level (passed in to the constructor). This is based on a C# implementation
// by Andreas Kahler (http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html). This sphere
// is used by my point light shader to tag vertices in the g-buffer affected by the light source.
class VESphere
{
	public :

		// ------- Public Functions -------

		// Construction
		VESphere( int aRefinementLevel = 3 );

		// Deconstruction
		~VESphere();

		// Builds the icosahedron mesh
		bool CreateMesh();

		// Loads the vertices and indices in to the input assembler
		void Prepare();


		// ---------- Accessors ----------

		int GetIndexCount()	{ return myIndices.size(); }


	private :

		// ------ Private Structures ------

		// Keeps track of faces when building the sphere
		struct FaceIndices
		{
			FaceIndices( int aVertex1, int aVertex2, int aVertex3 ) :
				myVertex1( aVertex1 ),
				myVertex2( aVertex2 ),
				myVertex3( aVertex3 )
			{
			}

			int myVertex1;
			int myVertex2;
			int myVertex3;
		};


		// ------- Private Functions ------

		// Creates the initial 12 vertices used in the icosahedron
		void	CreateInitialVertices();

		// Creates the initial faces of the icosahedron
		void	CreateInitialFaces( std::vector<FaceIndices>& someFaces );

		// Subdivides the triangles in the sphere mesh
		void	RefineFaces( std::vector<FaceIndices>& someFaces );

		// Adds a vertex to the mesh, fixing the position so it's a unit sphere. The index of
		// the new vertex is returned
		int		AddVertex( const DirectX::XMFLOAT3& aVertex );
		
		// Returns the index of the vertex in between the supplied vertices (the new vertex is added automagically)
		int		GetMiddlePoint( int aVertex1, int aVertex2 );

		// Creates the icosphere's vertex buffer
		bool	CreateVertexBuffer();

		// Creates the icosphere's index buffer
		bool	CreateIndexBuffer();


		// ------- Private Variables ------

		int								myRefinementLevel;

		std::vector<DirectX::XMFLOAT3>	myVertices;
		std::vector<int>				myIndices;
		std::map<INT64, int>			myMiddlePointIndexCache;

		ID3D11Buffer*					myVertexBuffer;
		ID3D11Buffer*					myIndexBuffer;
};


#endif
#ifndef VE_GEOMETRY_SHADER
#define VE_GEOMETRY_SHADER


// -------------------------- Includes -------------------------

#include "VEShader.h"


// -------------------------- Classes --------------------------

// A shader that renders colour, normals and depth to three separate buffers
class VEGeometryShader : public VEShader
{
	public :

		// ---------- Public Functions ----------

		// Construction
		VEGeometryShader();


	protected :

		// --------- Required Functions ---------

		// Creates the data layout used to send vertex information to the vertex shader
		virtual bool CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif // !VE_GEOMETRY_SHADER
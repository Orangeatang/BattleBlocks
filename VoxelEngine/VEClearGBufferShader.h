#ifndef VE_CLEAR_BUFFERS_SHADER_H
#define VE_CLEAR_BUFFERS_SHADER_H


// ------------------------- Includes -------------------------

#include "VEShader.h"


// ------------------------- Classes --------------------------

// A shader that clears the render targets used by the deferred render manager
class VEClearGBufferShader : public VEShader
{
	public :

		// ---------- Public Functions ----------

		// Construction 
		VEClearGBufferShader();

		// No vertex shader constants are required
		virtual bool PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override	{ return true; }

		// No pixel shader constants are required
		virtual bool PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override	{ return true; }


	protected :

		// --------- Required Functions ----------

		// Creates the data layout used to send information to the vertex shader
		virtual bool CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif // !VE_CLEAR_BUFFERS_SHADER_H
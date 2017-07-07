#ifndef VE_SHADOW_MAP_SHADER_H
#define VE_SHADOW_MAP_SHADER_H

// --------------------- Includes ---------------------

#include "VEShader.h"


// --------------------- Classes ----------------------

// A wrapper for the deferred point light shader used in deferred rendering
class VEShadowMapShader : public VEShader
{
	public :

		// ------ Public Functions ------

		// Construction
		VEShadowMapShader();


		// ---- Overridden Functions ----

		// Fills the vertex shader constant buffer
		virtual bool	PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override;

		// Fills the pixel shader constant buffer
		virtual bool	PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override { return true; } 


	protected :

		// ----- Required Functions -----

		// Creates the data layout used to send vertex information to the vertex shader
		virtual bool	CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif
#ifndef VE_FINAL_BLEND_H
#define VE_FINAL_BLEND_H


// --------------------- Includes ---------------------

#include "VEShader.h"


// --------------------- Classes ----------------------

// The final blend shader is used to blend the light buffer and the colour buffer 
// to produce the final output to the back buffer
class VEFinalBlendShader : public VEShader
{
	public :	

		// ------- Public Functions -------

		// Construction
		VEFinalBlendShader();


		// ----- Overridden Functions -----

		// Half pixel vertex shader constant
		virtual bool	PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override { return true; } 

		// Fills the pixel shader constant buffer
		virtual bool	PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override;


	protected :

		// ----- Protected Structures ------

		// The final blend shader needs to know the ambient light value
		struct PixelShaderConstants
		{
			DirectX::XMFLOAT4	myAmbientLight;
		};


		// ------ Required Functions -----

		// Creates the data layout used to send information to the vertex shader
		virtual bool CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif // !VE_FINAL_BLEND_H
#ifndef VE_SSAO_SHADER_H
#define VE_SSAO_SHADER_H


// ------------------------ Includes ------------------------

#include "VEShader.h"


// ------------------------ Classes -------------------------

// Wrapper for the deferred SSAO shader
class VESSAOShader : public VEShader
{
	public :

		// ------- Public Functions -------

		// Construction
		VESSAOShader();


		// ----- Overridden Functions -----

		// Since the SSAO pass is rendered using a full screen quad, no vertex shader constants needed
		virtual bool	PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override { return true; }

		// Fills in the pixel shader constant buffer
		virtual bool	PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override;


	protected :

		// ----- Required Functions -----

		// Creates the data layout used to send vertex information to the vertex shader
		virtual bool	CreateDataLayout( ID3D10Blob* aVertexShader ) override;


	private :

		// --- Protected Structures ----

		struct PixelShaderConstants
		{
			DirectX::XMFLOAT2 myScreenDimensions;
			DirectX::XMMATRIX myView;
			DirectX::XMMATRIX myInverseProjection;
		};
};


#endif // !VE_SSAO_SHADER_H
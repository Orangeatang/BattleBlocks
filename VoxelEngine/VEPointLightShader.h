#ifndef VE_POINT_LIGHT_SHADER_H
#define VE_POINT_LIGHT_SHADER_H


// ------------------ Includes ------------------

#include "VEShader.h"


// ------------------ Classes -------------------

// A wrapper for the deferred point light shader used in deferred rendering
class VEPointLightShader : public VEShader
{
	public :

		// ------ Public Functions ------

		// Construction
		VEPointLightShader();


		// ---- Overridden Functions ----

		// Fills the vertex shader constant buffer
		virtual bool	PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override;

		// Fills the pixel shader constant buffer
		virtual bool	PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override;

	
	protected :
	
		// ---- Protected Structures ----

		struct LightingConstants
		{
			 DirectX::XMFLOAT3			myLightPosition;
			 float						myRadius;
			 DirectX::XMFLOAT3			myLightColour;
			 float						myLightIntensity;
			 DirectX::XMMATRIX			myInverseViewProjection;
		};


		// ----- Required Functions -----

		// Creates the data layout used to send vertex information to the vertex shader
		virtual bool	CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif // !VE_POINT_LIGHT_SHADER
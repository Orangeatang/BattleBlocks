#ifndef VE_DIRECTIONAL_LIGHT_SHADER_H
#define VE_DIRECTIONAL_LIGHT_SHADER_H


// ------------------- Includes -------------------

#include "VEShader.h"


// ------------------- Classes --------------------

// A wrapper for the directional light shader used in deferred rendering. It uses the depth, normal & colour render targets
// to calculating a lighting value for each pixel
class VEDirectionalLightShader : public VEShader
{
	public :

		// ------ Public Functions ------

		// Construction
		VEDirectionalLightShader();


		// ---- Overridden Functions ----

		// Fills the vertex shader constant buffer
		virtual bool		PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override { return true; }

		// Fills the pixel shader constant buffer
		virtual bool		PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override;


	protected :

		// ---- Protected Structures ----

		// Lighting constants passed in to the pixel shader
		struct LightingConstants
		{
			DirectX::XMFLOAT3			myLightDirection;
			float						myApplyShadows;
			DirectX::XMFLOAT4			myLightColour;
			ALIGN_16 DirectX::XMFLOAT3	myCameraPosition;
			DirectX::XMMATRIX			myInverseViewProjection;
			DirectX::XMMATRIX			myLightViewProjection;
		};


		// ----- Required Functions -----

		// Creates the data layout used to send vertex information to the vertex shader
		virtual bool		CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif // !VE_DIRECTIONAL_LIGHT_SHADER_H
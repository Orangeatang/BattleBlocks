#ifndef VE_SPOT_LIGHT_SHADER_H
#define VE_SPOT_LIGHT_SHADER_H


// ------------------ Includes ------------------

#include "VEShader.h"


// ------------------ Classes -------------------

// A wrapper for the deferred spot light shader used in deferred rendering. The spot light shader
// is very similar to the point light shader, so it uses the same vertex shader
class VESpotLightShader : public VEShader
{
public :

	// ------ Public Functions ------

	// Construction
	VESpotLightShader();


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
		float						myLightRadius;
		DirectX::XMFLOAT3			myLightColour;
		float						myLightDecay;
		DirectX::XMFLOAT3			myLightDirection;
		float						myLightConeAngle;
		ALIGN_16 float				myApplyShadows;
		DirectX::XMMATRIX			myInverseViewProjection;
		DirectX::XMMATRIX			myLightViewProjection;
	};


	// ----- Required Functions -----

	// Creates the data layout used to send vertex information to the vertex shader
	virtual bool	CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif // !VE_SPOT_LIGHT_SHADER_H
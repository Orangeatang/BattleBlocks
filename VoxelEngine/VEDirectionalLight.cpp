
// -------------------- Includes --------------------

#include "Stdafx.h"
#include "VEDirectionalLight.h"


// -------------------- Namespaces ------------------

using namespace DirectX;


// ----------------- Class Functions ----------------

// Construction
VEDirectionalLight::VEDirectionalLight() : 
	VELight( LT_Directional ),
	myDirection( 0.0f, 0.0f, 0.0f )
{
}


// Creates an orthographic projection matrix for the light
void VEDirectionalLight::UpdateMatrices()
{
	XMStoreFloat4x4( &myProjectionMatrix, XMMatrixOrthographicLH(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, -SHADOW_MAP_SIZE, SHADOW_MAP_SIZE) );
	XMStoreFloat4x4( &myViewMatrix, XMMatrixLookAtLH( XMLoadFloat3(&myPosition), XMLoadFloat3(&myDirection), XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f))) );

	myIsDirty			= false;
}
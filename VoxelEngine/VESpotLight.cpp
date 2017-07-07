
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VESpotLight.h"


// --------------------- Namespaces ---------------------

using namespace DirectX;


// ------------------- Class Functions ------------------

// Construction
VESpotLight::VESpotLight() :
	VELight( LT_Spot ),
	myConeAngle( 45.0f ),
	myDecayRate( 10.0f ),
	myLightDirection( XMFLOAT3(0.0f, -1.0f, 0.0f) )
{
}


// Updates the view/projection matrices for the light
void VESpotLight::UpdateMatrices()
{
	float fov = (float)XM_PI / 2.0f;

	XMStoreFloat4x4( &myProjectionMatrix, XMMatrixPerspectiveFovLH(fov, 1.0f, 1.0f, SHADOW_MAP_SIZE) ); 
	XMStoreFloat4x4( &myViewMatrix, XMMatrixLookToLH(XMLoadFloat3(&myPosition), XMLoadFloat3(&myLightDirection), XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f))) );

	myIsDirty = false;
}
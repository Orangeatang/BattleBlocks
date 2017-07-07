
// ------------------ Includes ------------------

#include "Stdafx.h"
#include "VEBasicCamera.h"


// ----------------- Namespaces -----------------

using namespace DirectX;


// --------------- Class Functions --------------

// Construction
VEBasicCamera::VEBasicCamera( CameraType aType, float aNearPlane /* 0.1f */, float aFarPlane /* 10000.0f */ ) :
    myNearPlane( aNearPlane ),
    myFarPlane( aFarPlane ),
	myYaw( 0.0f ),
	myPitch( 0.0f ),
	myRoll( 0.0f ),
    myIsOrthographic( false ),
	myPositionDirty( false ),
	myRotationDirty( false ),
	myType( aType )
{
    XMStoreFloat4x4( &myWorld, XMMatrixIdentity() );
    XMStoreFloat4x4( &myView, XMMatrixIdentity() );
    XMStoreFloat4x4( &myPerspective, XMMatrixIdentity() );
    XMStoreFloat4x4( &myOrthographic, XMMatrixIdentity() );

    XMStoreFloat3( &myPosition, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f) );
    XMStoreFloat3( &myLookAt, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) );
    XMStoreFloat3( &myUp, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) );
}


// Initialises the world, view and projection matrices
bool VEBasicCamera::Initialise( int aScreenWidth, int aScreenHeight )
{
    // Initialise the view and projection matrices
    UpdateProjectionMatrices( aScreenWidth, aScreenHeight );
    Update();

    // Initialise the world matrix
    XMStoreFloat4x4( &myWorld, XMMatrixIdentity() );

    return true;
}


// Sets both the orthographic and perspective projection matrices
void VEBasicCamera::UpdateProjectionMatrices( int aScreenWidth, int aScreenHeight )
{
    // Create the perspective projection matrix
    float fov             = (float)XM_PI / 4.0f;
    float screenAspect    = (float)aScreenWidth / (float)aScreenHeight;
	XMStoreFloat4x4( &myPerspective, XMMatrixPerspectiveFovLH(fov, screenAspect, myNearPlane, myFarPlane) );

    // Create the orthographic projection matrix
    XMStoreFloat4x4( &myOrthographic, XMMatrixOrthographicLH((float)aScreenWidth, (float)aScreenHeight, -myFarPlane, myFarPlane) );
}


// Updates the view matrix based on the camera position, look-at and rotation
void VEBasicCamera::Update()
{
	if( myPositionDirty )
	{
		UpdatePosition();
	}
	else if( myRotationDirty )
	{
		UpdateRotation();
	}
}
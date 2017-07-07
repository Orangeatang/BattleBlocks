
// ------------------- Includes -------------------

#include "Stdafx.h"
#include "VEFreeCamera.h"


// ------------------ Namespaces ------------------

using namespace DirectX;


// --------------- Class Functions ----------------

// Construction
VEFreeCamera::VEFreeCamera( CameraType aCameraType /*CT_Free*/, float aNearPlane /*0.1f*/, float aFarPlane /*10000.0f*/ ) : VEFirstPersonCamera( aCameraType, aNearPlane, aFarPlane )
{
	XMStoreFloat3( &myCameraForward, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) );
	XMStoreFloat3( &myCameraRight, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) );
}


// Simulates forward/back movement along the camera axis
void VEFreeCamera::SimulateMedialMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition )
{
	XMMATRIX rotationY = XMMatrixRotationY( myYaw );
	XMMATRIX rotationX = XMMatrixRotationX( myPitch );

	XMStoreFloat3( &myCameraForward, XMVector3TransformCoord(XMLoadFloat3(&myForward), rotationY *  rotationX) );
	XMStoreFloat3( &myUp, XMVector3Transform(XMLoadFloat3(&myUp), rotationY) ); 

	XMVECTOR newPosition = XMLoadFloat3( &myPosition );
	newPosition += aSpeed * XMLoadFloat3(&myCameraForward);
	
	XMStoreFloat3( &aNewPosition, newPosition );
}


// SImulates left/right movement along the camera axis
void VEFreeCamera::SimulateLateralMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition )
{
	XMMATRIX rotationY = XMMatrixRotationY( myYaw );
	XMMATRIX rotationX = XMMatrixRotationX( myPitch );

	XMStoreFloat3( &myCameraRight, XMVector3TransformCoord(XMLoadFloat3(&myRight), rotationX * rotationY) );
	XMStoreFloat3( &myUp, XMVector3Transform(XMLoadFloat3(&myUp), rotationY) );

	XMVECTOR newPosition = XMLoadFloat3( &myPosition );
	newPosition += aSpeed * XMLoadFloat3( &myCameraRight );

	XMStoreFloat3( &aNewPosition, newPosition );
}


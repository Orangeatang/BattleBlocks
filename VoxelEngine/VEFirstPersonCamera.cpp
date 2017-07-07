
// ------------------- Includes -------------------

#include "Stdafx.h"
#include "VEFirstPersonCamera.h"


// ------------------- Namespaces -----------------

using namespace DirectX;


// ---------------- Class Functions ---------------

// Construction
VEFirstPersonCamera::VEFirstPersonCamera( CameraType aCameraType /*CT_FirstPerson*/, float aNearPlane /*0.1f*/, float aFarPlane /*10000.0f*/ ) : VEBasicCamera( aCameraType, aNearPlane, aFarPlane )
{	
	XMStoreFloat4x4( &myRotation, XMMatrixIdentity() );
	XMStoreFloat3( &myForward, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) ) ;
	XMStoreFloat3( &myRight, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) );
}


// Calculates forward/back movement result given a speed and the current state of the camera
void VEFirstPersonCamera::SimulateMedialMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition )
{
	// Calculate the forward direction for the camera
	XMFLOAT3 forward;
	XMMATRIX currentRotation = XMMatrixRotationY( myYaw );
	XMStoreFloat3( &forward, XMVector3Transform(XMLoadFloat3(&myForward), currentRotation) );

	// Move the current position in the direction that the camera is facing
	XMVECTOR newPosition = XMLoadFloat3( &myPosition );
	newPosition += aSpeed * XMLoadFloat3( &forward );
	XMStoreFloat3( &aNewPosition, newPosition );
}


// Calculates left/right movement result given a speed and the current state of the camera
void VEFirstPersonCamera::SimulateLateralMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition )
{
	// Calculate the left/right direction for the camera
	XMFLOAT3 leftRight;
	XMMATRIX rotation = XMMatrixRotationY( myYaw );
	XMStoreFloat3( &leftRight, XMVector3TransformCoord(XMLoadFloat3(&myRight), rotation) );

	// Move the current position of the camera along the horizontal axis
	XMVECTOR newPosition = XMLoadFloat3( &myPosition );
	newPosition += aSpeed * XMLoadFloat3( &leftRight );
	XMStoreFloat3( &aNewPosition, newPosition );
}


// Updates the position & target for the view matrix
void VEFirstPersonCamera::UpdatePosition()
{
	XMVECTOR cameraPosition = XMLoadFloat3(&myPosition);
	XMVECTOR cameraTarget   = XMVector3Transform( XMLoadFloat3(&myForward), XMLoadFloat4x4(&myRotation) );

	// Update the camera target & view matrix
	cameraTarget = cameraPosition + cameraTarget;
	XMStoreFloat4x4( &myView, XMMatrixLookAtLH(cameraPosition, cameraTarget, XMLoadFloat3(&myUp)) );

	VEBasicCamera::UpdatePosition();
}


// Updates the rotation for the view matrix
void VEFirstPersonCamera::UpdateRotation()
{
	// Recalculate the rotation of the camera
	XMStoreFloat4x4( &myRotation, XMMatrixRotationRollPitchYaw(myPitch, myYaw, myRoll) );
	XMVECTOR cameraPosition = XMLoadFloat3(&myPosition);
	XMVECTOR cameraTarget   = XMVector3Transform( XMLoadFloat3(&myForward), XMLoadFloat4x4(&myRotation) );

	// Update the view matrix
	cameraTarget = cameraPosition + cameraTarget;
	XMStoreFloat4x4( &myView, XMMatrixLookAtLH(cameraPosition, cameraTarget, XMLoadFloat3(&myUp)) );

	VEBasicCamera::UpdateRotation();
}
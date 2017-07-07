#ifndef VE_FREE_CAMERA_H
#define VE_FREE_CAMERA_H


// ------------------- Includes -------------------

#include "VEFirstPersonCamera.h"


// ------------------- Classes --------------------

// A free camera implementation of the basic camera
class VEFreeCamera : public VEFirstPersonCamera
{
	public :

		// ----- Public Functions -----

		// Construction
		VEFreeCamera( CameraType aCameraType = CT_Free, float aNearPlane = 0.1f, float aFarPlane = 10000.0f );


		// ----- Overridden Functions------

		virtual void SimulateMedialMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition ) override;
		
		virtual void SimulateLateralMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition ) override;

	
	protected :

		// ----- Protected Variables -----

		DirectX::XMFLOAT3	myCameraForward;
		DirectX::XMFLOAT3	myCameraRight;
};


#endif // !VE_FREE_CAMERA_H
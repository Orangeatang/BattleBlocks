#ifndef FIRST_PERSON_CAMERA_H
#define FIRST_PERSON_CAMERA_H

// ------------------- Includes -------------------

#include "VEBasicCamera.h"


// ------------------- Classes --------------------

// A first person implementation of the VoxelEngine's basic camera class
class VEFirstPersonCamera : public VEBasicCamera
{
	public :

		// ---------- Public Functions ----------

		// Construction
		VEFirstPersonCamera( CameraType aCameraType = CT_FirstPerson, float aNearPlane = 0.1f, float aFarPlane = 10000.0f );

		// Calculates forward/back movement result given a speed and the current state of the camera
		virtual void	SimulateMedialMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition );

		// Calculates left/right movement result given a speed and the current state of the camera
		virtual void	SimulateLateralMovement( float aSpeed, DirectX::XMFLOAT3& aNewPosition );


	protected :

		// --------- Protected Function ---------

		// Updates the position & target for the view matrix
		virtual void	UpdatePosition() override;
		
		// Updates the rotation for the view matrix
		virtual void	UpdateRotation() override;


		// --------- Protected Variables --------

		DirectX::XMFLOAT3	myForward;
		DirectX::XMFLOAT3	myRight;
		DirectX::XMFLOAT4X4	myRotation;
};


#endif // !FIRST_PERSON_CAMERA_H
#ifndef VE_BASIC_CAMERA_H
#define VE_BASIC_CAMERA_H


// ------------------ Includes ------------------

#include "VETypes.h"


// ------------------ Classes -------------------

// A simple camera implementation, with a position, rotation and associated matrices
class VEBasicCamera
{
    public :

        // -------- Public Functions --------

        // Construction
        VEBasicCamera( CameraType aType, float aNearPlane = 0.1f, float aFarPlane = 10000.0f );

        // Initialises the world, view and projection matrices
        virtual bool				Initialise( int aScreenWidth, int aScreenHeight );

        // Sets both the orthographic and perspective projection matrices
        virtual void				UpdateProjectionMatrices( int aScreenWidth, int aScreenHeight );

        // Updates the camera matrices
        void						Update();


        // ----------- Accessors -----------

		CameraType					GetType()										{ return myType; }

		const DirectX::XMFLOAT3&	GetPosition()									{ return myPosition; }
        void						SetPosition( DirectX::XMFLOAT3& aNewPosition )	{ myPosition = aNewPosition; myPositionDirty = true; }

		float						GetYaw()										{ return myYaw; }
		void						AddYaw( float aYaw )							{ myYaw += aYaw; myRotationDirty = true; }

		float						GetPitch()										{ return myPitch; }
		void						AddPitch( float aPitch )						{ myPitch += aPitch; myRotationDirty = true; }

		float						GetRoll()										{ return myRoll; }
		void						AddRoll( float aRoll )							{ myRoll += aRoll; myRotationDirty = true; }

        const DirectX::XMFLOAT3&	GetLookAt()										{ return myLookAt; }
        void						SetLookAt( DirectX::XMFLOAT3& aNewLookAt )		{ myLookAt = aNewLookAt; myRotationDirty = true; }

        const DirectX::XMFLOAT3&	GetUp()											{ return myUp; }
        void						SetUp( DirectX::XMFLOAT3& aNewUp )				{ myUp = aNewUp; myRotationDirty = true; }

        DirectX::XMFLOAT4X4&		GetWorld()										{ return myWorld; }
        
        // Returns the view matrix, updating it if need be
        DirectX::XMFLOAT4X4&		GetView()
        {
            if( myPositionDirty || myRotationDirty )
            {
                Update();
            }

            return myView;
        }

        // Returns the projection matrix, based on the projection type
        DirectX::XMFLOAT4X4&		GetProjection()
        {
            if( myIsOrthographic )
            {
                return myOrthographic;
            }

            return myPerspective;
        }


	protected: 
        
		// ------ Protected Functions -------

		// Child classes should use this to update the view matrix position/target
		virtual void				UpdatePosition()	{ myPositionDirty = false; }

		// Child classes should use this to update the view matrix rotation
		virtual void				UpdateRotation()	{ myRotationDirty = false; }
        
		
		// ------ Protected Variables -------

		bool				myPositionDirty;
		bool				myRotationDirty;

        float				myNearPlane;
        float				myFarPlane;

		float				myYaw;
		float				myPitch;
		float				myRoll;

        bool				myIsOrthographic;

        DirectX::XMFLOAT4X4	myWorld;
        DirectX::XMFLOAT4X4	myView;
        DirectX::XMFLOAT4X4	myPerspective;
        DirectX::XMFLOAT4X4	myOrthographic;

        DirectX::XMFLOAT3	myPosition;
        DirectX::XMFLOAT3	myLookAt;
        DirectX::XMFLOAT3	myUp;

	
	private :

		// ------ Private Variables ------

		CameraType			myType;
};

#endif // !VE_BASIC_CAMERA_H
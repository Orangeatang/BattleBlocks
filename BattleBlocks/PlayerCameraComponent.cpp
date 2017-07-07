
// ------------------------- Includes -------------------------

#include "Stdafx.h"
#include "PlayerCameraComponent.h"

#include "Types.h"
#include "MouseMoveMessage.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VEBasicCamera.h"
#include "VEFirstPersonCamera.h"
#include "VEFreeCamera.h"
#include "VEObject.h"


// -------------------------- Typedefs ------------------------

typedef std::vector<VESmartPointer<VEComponentMessage>>::iterator MessageIterator;


// ------------------------- Namespaces -----------------------

using namespace DirectX;


// ---------------------- Class Functions ---------------------

// Construction
PlayerCameraComponent::PlayerCameraComponent( VEObject* aParent ) : VEObjectComponent( GC_Camera, aParent ),
	myCamera( NULL )
{
}


// Sets the current camera for the player & engine
void PlayerCameraComponent::SetCamera( CameraType aCameraType )
{
	VoxelEngine* voxelEngine = VoxelEngine::GetInstance();
	assert( voxelEngine != NULL );

	VEDirectXInterface* renderInterface = voxelEngine->GetRenderInterface();
	assert( renderInterface != NULL );

	XMFLOAT3 position = myParent->GetPosition();
	XMFLOAT3 lookat( 0.0f, 0.0f, 1.0f );

	if( myCamera != NULL )
	{
		position = myCamera->GetPosition();
		lookat   = myCamera->GetLookAt();

		voxelEngine->SetCamera( NULL );
		
		delete myCamera;
		myCamera = NULL;
	}

	switch( aCameraType )
	{
		case CT_Free :
			myCamera = new VEFreeCamera();
			break;

		case CT_FirstPerson :
			myCamera = new VEFirstPersonCamera();
			break;

		case CT_Basic :
		default :
			myCamera = new VEBasicCamera( CT_Basic );
			break;
	}

	assert( myCamera != NULL );
	myCamera->Initialise( renderInterface->GetScreenWidth(), renderInterface->GetScreenHeight() );
	myCamera->SetLookAt( lookat );
	myCamera->SetPosition( position );
	voxelEngine->SetCamera( myCamera );
}


// Initialises the camera
bool PlayerCameraComponent::Initialise()
{
	SetCamera( CT_FirstPerson );

	AddMessageType( MT_MouseMove );

	return true;
}


// Cleans up the memory used by the camera
void PlayerCameraComponent::Cleanup()
{
	if( myCamera != NULL )
	{
		delete myCamera;
		myCamera = NULL;
	}
}


// Processes messages in the queue
void PlayerCameraComponent::ProcessMessages( float anElapsedTime )
{
	for( MessageIterator msg = myMessageQueue.begin(); msg != myMessageQueue.end(); msg++ )
	{
		switch( (*msg)->GetType() )
		{
			case MT_MouseMove :
				{
					MouseMoveMessage* moveMessage = dynamic_cast<MouseMoveMessage*>( &(*msg) );
					HandleMouseMoveMessage( moveMessage, anElapsedTime );
				}
				break;

			default :
				break;
		}

		(*msg) = NULL;
	}

	myMessageQueue.clear();
}


// Applies data from the move message to the active camera
void PlayerCameraComponent::HandleMouseMoveMessage( MouseMoveMessage* aMoveMessage, float anElapsedTime )
{
	assert( aMoveMessage != NULL );
	switch( aMoveMessage->GetAxis() )
	{
		case MouseMoveMessage::MX_Pitch :
			{
				myCamera->AddPitch( aMoveMessage->GetDelta() * anElapsedTime );
			}
			break;

		case MouseMoveMessage::MX_Yaw :
			{
				myCamera->AddYaw( aMoveMessage->GetDelta() * anElapsedTime );
			}
			break;

		default :
			break;
	}
}
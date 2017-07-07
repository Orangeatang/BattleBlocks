
// -------------------- Includes --------------------

#include "Stdafx.h"
#include "PlayerPhysicsComponent.h"

#include "Types.h"
#include "MoveEventMessage.h"

#include "VEObject.h"
#include "VoxelEngine.h"
#include "VEFirstPersonCamera.h"
#include "VEPhysicsService.h"
#include "VEFirstPersonCamera.h"


// -------------------- Typedefs --------------------

typedef std::vector<VESmartPointer<VEComponentMessage>>::iterator MessageIterator;


// -------------------- Namespaces ------------------

using namespace DirectX;


// ----------------- Class Functions ----------------

// Construction
PlayerPhysicsComponent::PlayerPhysicsComponent( VEObject* aParentObject ) : VEPhysicsComponent( aParentObject, GC_PlayerPhysics ),
	myLateralSpeed( 10.0f ),
	myMedialSpeed( 10.0f ),
	mySpeedModifier( 1.0f )
{
}


// Processes incoming messages and updates the position of the player/camera
void PlayerPhysicsComponent::Update( float anElapsedTime )
{
	VEPhysicsComponent::Update( anElapsedTime );

	// Apply movement calculations
	ProcessMovement( anElapsedTime );
}


// Registers the supported messages
bool PlayerPhysicsComponent::Initialise()
{
	AddMessageType( MT_BeginMovement );
	AddMessageType( MT_EndMovement );

	return VEPhysicsComponent::Initialise();
}


// Processing incoming movement messages
void PlayerPhysicsComponent::ProcessMessages( float anElapsedTime )
{
	for( MessageIterator msg = myMessageQueue.begin(); msg != myMessageQueue.end(); msg++ )
	{
		switch( (*msg)->GetType() )
		{
			case MT_BeginMovement :
				{
					MoveEventMessage* moveMessage = dynamic_cast<MoveEventMessage*>( &(*msg) );
					assert( moveMessage != NULL );

 					HandleBeginMoveMessage( moveMessage );
				}
				break;

			case MT_EndMovement :
				{
					MoveEventMessage* endMessage = dynamic_cast<MoveEventMessage*>( &(*msg) );
					assert( endMessage != NULL );

					HandleEndMoveMessage( endMessage );
				}

			default :
				break;
		}

		(*msg) = NULL;
	}

	myMessageQueue.clear();
}


// Handles movement requests
void PlayerPhysicsComponent::HandleBeginMoveMessage( MoveEventMessage* aMoveMessage )
{
	VEFirstPersonCamera*	camera		= NULL;
	switch( VoxelEngine::GetInstance()->GetCameraType() )
	{
		case CT_Free :
		case CT_FirstPerson :
			camera = dynamic_cast<VEFirstPersonCamera*>( VoxelEngine::GetInstance()->GetCamera() );
			break;

		default :
			return;
	}

	XMFLOAT3 newPosition;
	XMFLOAT3 currentPosition = camera->GetPosition();
	switch( aMoveMessage->GetPlayerAction() )
	{
		case PA_MoveForward :
			myVelocity.z = 1.0f;
			break;

		case PA_MoveBack :
			myVelocity.z = -1.0f;
			break;

		case PA_StrafeLeft :
			myVelocity.x = -1.0f;
			break;

		case PA_StrafeRight :
			myVelocity.x = 1.0f;
			break;

		case PA_Sprint :
			if( !IsInAir() )
			{
				mySpeedModifier = 2.0f;
			}
			break;

		case PA_Walk :
			if( !IsInAir() )
			{
				mySpeedModifier = 0.5f;
			}
			break;

		case PA_Jump :
			myVelocity.y = 10.0f;
			break;

		default :
			break;
	}
}


// Processes incoming 'end move' messages
void PlayerPhysicsComponent::HandleEndMoveMessage( MoveEventMessage* aMoveMessage )
{
	// Disable movement for the action defined in the message
	switch( aMoveMessage->GetPlayerAction() )
	{
		case PA_MoveForward :
			{
				if( myVelocity.z > 0.0f )
				{
					myVelocity.z = 0.0f;
				}
			}
			break;

		case PA_MoveBack :
			{
				if( myVelocity.z < 0.0f )
				{
					myVelocity.z = 0.0f;
				}
			}
			break;

		case PA_StrafeLeft :
			{
				if( myVelocity.x < 0.0f )
				{
					myVelocity.x = 0.0f;
				}
			}
			break;

		case PA_StrafeRight :
			{
				if( myVelocity.x > 0.0f )
				{
					myVelocity.x = 0.0f;
				}
			}
			break;

		case PA_Sprint :
			{
				if( !IsInAir() && mySpeedModifier > 1.0f )
				{
					mySpeedModifier = 1.0f;
				}
			}
			break;

		case PA_Walk :
			{
				if( !IsInAir() && mySpeedModifier < 1.0f )
				{
					mySpeedModifier = 1.0f;
				}
			}
			break;
			
		default :
			break;
	};
}


// Moves the player/camera around in the world based on active movement directions
void PlayerPhysicsComponent::ProcessMovement( float anElapsedTime )
{
	VEPhysicsService* physicsService = VoxelEngine::GetInstance()->GetPhysicsService();
	assert( physicsService != NULL );

	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEFirstPersonCamera* camera = dynamic_cast<VEFirstPersonCamera*>( VoxelEngine::GetInstance()->GetCamera() );
	assert( camera != NULL );

	bool	 positionUpdated = false;
	XMFLOAT3 targetPosition = myParent->GetPosition();
	
	if( myVelocity.z != 0.0f )
	{
		camera->SimulateMedialMovement( (myVelocity.z * (myMedialSpeed * mySpeedModifier)) * anElapsedTime, targetPosition );
		if( physicsService->ValidateMovement(camera->GetPosition(), targetPosition) )
		{
			positionUpdated = true;
		}
	}

	if( myVelocity.x != 0.0f )
	{
		camera->SimulateLateralMovement( (myVelocity.x * (myLateralSpeed * mySpeedModifier)) * anElapsedTime, targetPosition );
		if( physicsService->ValidateMovement(camera->GetPosition(), targetPosition) )
		{
			positionUpdated = true;
		}
	}

	if( myVelocity.y != 0.0f )
	{
		targetPosition.y += myVelocity.y * anElapsedTime;
		positionUpdated = true;
 	}

	if( positionUpdated )
	{
		myParent->SetPosition( targetPosition );
		camera->SetPosition( targetPosition );
	}
}
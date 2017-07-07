
// ------------------------- Includes -------------------------

#include "Stdafx.h"
#include "PlayerInputComponent.h"

#include "VoxelEngine.h"
#include "VEDirectXInput.h"

#include "MoveEventMessage.h"
#include "MouseMoveMessage.h"


// -------------------------- Typedefs ------------------------

typedef std::pair<PlayerAction, PlayerInputComponent::ActiveAction> ActionMap;


// -------------------------- Structs -------------------------

// Used for retrieving keys from a std::map
struct GetKey
{
	template<typename T>
	typename T::first_type operator()( T aKeyValuePair ) const
	{
		return aKeyValuePair.first;
	}
};


// ---------------------- Class Functions ---------------------

// Construction
PlayerInputComponent::PlayerInputComponent( VEObject* aParent ) : VEObjectComponent( GC_PlayerInput, aParent ),
	myCurrentMovementSpeed( 10.0f ),
	myPitchSensitivity( 0.05f ),
	myYawSensitivity( 0.05f )
{
}


// Deconstruction
PlayerInputComponent::~PlayerInputComponent()
{
}


// Handles per-frame updates of the input component
void PlayerInputComponent::Update( float anElapsedTime )
{
	VEObjectComponent::Update( anElapsedTime );

	ProcessKeyboardInput();
	ProcessMouseInput();

	// Send the messages
	for( std::vector<VEComponentMessage*>::iterator msg = myMessages.begin(); msg != myMessages.end(); msg++ )
	{
		if( (*msg)->IsValid() )
		{
			SendMessage( (*msg) );
		}
		else
		{
			delete (*msg);
			(*msg) = NULL;
		}
	}
	myMessages.clear();
}


// Initialises the key to action maps
bool PlayerInputComponent::Initialise()
{
	myKeyboardActionMap.insert( ActionMap(PA_MoveForward, ActiveAction(DIK_W)) );
	myKeyboardActionMap.insert( ActionMap(PA_MoveBack, ActiveAction(DIK_S)) );
	myKeyboardActionMap.insert( ActionMap(PA_StrafeLeft, ActiveAction(DIK_A)) );
	myKeyboardActionMap.insert( ActionMap(PA_StrafeRight, ActiveAction(DIK_D)) );
	myKeyboardActionMap.insert( ActionMap(PA_Sprint, ActiveAction(DIK_LSHIFT)) );
	myKeyboardActionMap.insert( ActionMap(PA_Walk, ActiveAction(DIK_LCONTROL)) );
	myKeyboardActionMap.insert( ActionMap(PA_Jump, ActiveAction(DIK_SPACE)) );

	return true;
}


// Processes messages in the message queue
void PlayerInputComponent::ProcessMessages( float anElapsedTime )
{

}


// Checks the keyboard for any input events
void PlayerInputComponent::ProcessKeyboardInput()
{
	VEDirectXInput* directInput = VoxelEngine::GetInstance()->GetInputInterface();
	assert( directInput != NULL );

	// Get the keys for the player actions
	std::vector<unsigned int> keys;
	std::transform( myKeyboardActionMap.begin(), myKeyboardActionMap.end(), std::back_inserter(keys), GetKey() );

	// Loop through the player actions, creating the appropriate move/end move messages
	for( std::vector<unsigned int>::iterator action = keys.begin(); action != keys.end(); action++ )
	{
		PlayerAction currentAction = (PlayerAction)(*action);

		// If the key is down and the player action isn't active, send a begin move message
		if( directInput->IsKeyDown(myKeyboardActionMap[currentAction].myKey) )
		{
			if( !myKeyboardActionMap[currentAction].myActive )
			{
				myMessages.push_back( new MoveEventMessage(MT_BeginMovement, currentAction) );
				myKeyboardActionMap[currentAction].myActive = true;
			}
		}
		// If the key is up and the player action is active, send an end move message
		else if( myKeyboardActionMap[currentAction].myActive )
		{
			myMessages.push_back( new MoveEventMessage(MT_EndMovement, currentAction) );
			myKeyboardActionMap[currentAction].myActive = false;
		}
	}
}


// Checks the mouse for any input events
void PlayerInputComponent::ProcessMouseInput()
{
	VEDirectXInput* directInput = VoxelEngine::GetInstance()->GetInputInterface();
	assert( directInput != NULL );

	// Check the pitch & yaw of the mouse to see if any message processing is required
	if( directInput->MouseDeltaY() != 0 )
	{
		myMessages.push_back( new MouseMoveMessage(MouseMoveMessage::MX_Pitch, (float)directInput->MouseY() * myYawSensitivity) );
	}

	if( directInput->MouseDeltaX() != 0 )
	{
		myMessages.push_back( new MouseMoveMessage(MouseMoveMessage::MX_Yaw, (float)directInput->MouseX() * myPitchSensitivity) );
	}
}
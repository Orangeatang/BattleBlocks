
// --------------------- Includes ---------------------

#include "Stdafx.h"
#include "LocalPlayer.h"

#include "PlayerInputComponent.h"
#include "PlayerCameraComponent.h"
#include "PlayerPhysicsComponent.h"


// ------------------ Class Functions -----------------

// Construction
LocalPlayer::LocalPlayer() : 
	VEObject()
{
}


// Initialises the components used by the local player
bool LocalPlayer::Initialise( bool isDynamic /* = true */ )
{
	VEObject::Initialise( isDynamic );
	myPosition = DirectX::XMFLOAT3( 0.0f, 30.0f, 0.0f );

	// Add the components
	PlayerPhysicsComponent* physics = new PlayerPhysicsComponent( this );
	physics->Initialise();
	AddComponent( physics );
 	
	PlayerInputComponent* playerInput = new PlayerInputComponent( this );
	playerInput->Initialise();
	AddComponent( playerInput );

	PlayerCameraComponent* camera = new PlayerCameraComponent( this );
	camera->Initialise();
	AddComponent( camera );

	// Connect up the message handlers
	playerInput->AddMessageTarget( physics );
	playerInput->AddMessageTarget( camera );

	return true;
}
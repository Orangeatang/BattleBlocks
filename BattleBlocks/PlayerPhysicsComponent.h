#ifndef PLAYER_PHYSICS_COMPONENT_H
#define PLAYER_PHYSICS_COMPONENT_H


// -------------------- Includes --------------------

#include "VEPhysicsComponent.h"


// --------------- Forward Declarations -------------

class MoveEventMessage;


// -------------------- Classes ---------------------

// A player specific physics component that handles movement request messages
class PlayerPhysicsComponent : public VEPhysicsComponent 
{
	public :

		// ----- Public Functions -----

		// Construction
		PlayerPhysicsComponent( VEObject* aParentObject );

		// Processes incoming messages and updates the position of the player/camera
		virtual void	Update( float anElapsedTime ) override;


		// ---- Required Functions ----

		// Registers the supported messages
		virtual bool	Initialise() override;


		// -------- Accessors ---------

		float			GetLateralSpeed()						{ return myLateralSpeed; }
		void			SetLateralSpeed( float aLateralSpeed )	{ myLateralSpeed = aLateralSpeed; }

		float			GetMedialSpeed()						{ return myMedialSpeed; }
		void			SetMedialSpeed( float aMedialSpeed )	{ myMedialSpeed = aMedialSpeed; }


	protected :

		// ---- Required Functions ----

		virtual void	ProcessMessages( float anElapsedTime ) override;


	private :

		// ----- Private Functions ----

		// Processes incoming 'begin move' messages
		void			HandleBeginMoveMessage( MoveEventMessage* aMoveMessage );

		// Processes incoming 'end move' messages
		void			HandleEndMoveMessage( MoveEventMessage* aMoveMessage );

		// Moves the player/camera around in the world based on active movement directions
		void			ProcessMovement( float anElapsedTime );


		// ----- Private Variables ----

		float				myLateralSpeed;
		float				myMedialSpeed;
		float				mySpeedModifier;
};


#endif // !PLAYER_PHYSICS_COMPONENT_H
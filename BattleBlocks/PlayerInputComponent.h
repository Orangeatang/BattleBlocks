#ifndef PLAYER_INPUT_COMPONENT_H
#define PLAYER_INPUT_COMPONENT_H


// ------------------------- Includes -------------------------

#include "VEObjectComponent.h"

#include "Types.h"


// ------------------------- Classes --------------------------

// Interacts with the DirectX to detect input from the keyboard/mouse, and sends the appropriate messages to
// any registered object components
class PlayerInputComponent : public VEObjectComponent
{
	public :
		
		// ----- Public Functions -----

		// Construction
		PlayerInputComponent( VEObject* aParent );

		// Deconstruction
		~PlayerInputComponent();

		// Handles per-frame updates of the input component
		virtual void Update( float anElapsedTime ) override;


		// ---- Required Functions ----

		// // Initialises the key to action maps
		virtual bool Initialise();

		// Placeholder
		virtual void Cleanup() override		{}


		// -------- Accessors ---------

		float		GetMovementSpeed()							{ return myCurrentMovementSpeed; }
		void		SetMovementSpeed( float aMovementSpeed )	{ myCurrentMovementSpeed = aMovementSpeed; }

		void		SetPitchSensitivity( float aSensitivity )	{ myPitchSensitivity = aSensitivity; }
		float		GetPitchSensitivity()						{ return myPitchSensitivity; }

		void		SetYawSensitivity( float aSensitivity )		{ myYawSensitivity = aSensitivity; }
		float		GetYawSensitivity()							{ return myYawSensitivity; }


		// ---- Public Structures ----

		// Used to keep track of whether a player action is active/inactive
		struct ActiveAction
		{
			ActiveAction( unsigned int aKey ) :
				myKey( aKey ),
				myActive( false )
			{
			}

			ActiveAction() :
				myKey( -1 ),
				myActive( false )
			{
			}


			unsigned int myKey;
			bool		 myActive;
		};


	protected :

		// ---- Required Functions ----

		// Processes messages in the message queue
		virtual void ProcessMessages( float anElapsedTime ) override;


	private :

		// ---- Private Functions ----

		// Checks the keyboard for any input events
		void		ProcessKeyboardInput();		

		// Checks the mouse for any input events
		void		ProcessMouseInput();

		// Checks the gamepad for any input events
		void		ProcessGamepadInput();


		// ---- Private Variables ----

		std::map<PlayerAction, ActiveAction>	myKeyboardActionMap;
		float									myCurrentMovementSpeed;
		float									myPitchSensitivity;
		float									myYawSensitivity;

		std::vector<VEComponentMessage*>		myMessages;
};


#endif // !PLAYER_INPUT_COMPONENT_H
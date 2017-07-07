#ifndef MOVE_MESSAGE_H
#define MOVE_MESSAGE_H


// -------------------- Includes --------------------

#include "VEComponentMessage.h"

#include "Types.h"


// -------------------- Classes ---------------------

// A message that requests movement of an object
class BeginMovementMessage : public VEComponentMessage
{
	public :

		// ------ Public Functions ------

		// Construction
		BeginMovementMessage() : VEComponentMessage( MT_BeginMovement ),
			myMovementType( PA_Max ),
			myMovementSpeed( 0.0f )
		{
		}

		BeginMovementMessage( PlayerAction anAction ) : VEComponentMessage( MT_BeginMovement ),
			myMovementType( anAction ),
			myMovementSpeed( 0.0f )
		{
		}


		// --------- Accessors ----------

		void			SetMovementType( PlayerAction aMovementType )	{ myMovementType = aMovementType; }
		PlayerAction	GetMovementType()								{ return myMovementType; }

		void			SetMovementSpeed( float aMovementSpeed )		{ myMovementSpeed = aMovementSpeed; }
		const float		GetMovementSpeed()								{ return myMovementSpeed; }

		virtual bool	IsValid() override								{ return (myMovementSpeed != 0.0f && myMovementType != PA_Max); }


	private :
	
		// ------ Private Variables ------

		PlayerAction	myMovementType;
		float			myMovementSpeed;
};


#endif // !MOVE_MESSAGE_H
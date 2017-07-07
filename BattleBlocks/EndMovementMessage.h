#ifndef END_MOVEMENT_MESSAGE_H
#define END_MOVEMENT_MESSAGE_H


// -------------------- Includes --------------------

#include "VEComponentMessage.h"

#include "Types.h"


// -------------------- Classes ---------------------

// A message that stops movement of an object
class EndMovementMessage : public VEComponentMessage
{
	public :

		// ------ Public Functions ------

		// Construction
		EndMovementMessage() : VEComponentMessage( MT_EndMovement ),
			myMovementType( PA_Max )
		{
		}

		EndMovementMessage( PlayerAction aPlayerAction ) : VEComponentMessage( MT_EndMovement ),
			myMovementType( aPlayerAction )
		{
		}


		// --------- Accessors ----------

		void			SetMovementType( PlayerAction aMovementType )	{ myMovementType = aMovementType; }
		PlayerAction	GetMovementType()								{ return myMovementType; }


		virtual bool	IsValid() override								{ return myMovementType != PA_Max; }


	private :

		// ------ Private Variables ------

		PlayerAction	myMovementType;
};


#endif // !END_MOVEMENT_MESSAGE
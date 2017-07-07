#ifndef MOVE_EVENT_MESSAGE_H
#define MOVE_EVENT_MESSAGE_H


// -------------------- Includes --------------------

#include "VEComponentMessage.h"

#include "Types.h"


// -------------------- Classes ---------------------

// A message that stops/starts movement of an object (depending on the supplied message type)
class MoveEventMessage : public VEComponentMessage
{
	public :

		// ------ Public Functions ------

		// Construction
		MoveEventMessage( MessageType aMessageType ) : VEComponentMessage( aMessageType ),
			myPlayerAction( PA_Max )
		{
		}

		MoveEventMessage( MessageType aMessageType, PlayerAction aPlayerAction ) : VEComponentMessage( aMessageType ),
			myPlayerAction( aPlayerAction )
		{
		}


		// --------- Accessors ----------

		void			SetPlayerAction( PlayerAction aMovementType )	{ myPlayerAction = aMovementType; }
		PlayerAction	GetPlayerAction()								{ return myPlayerAction; }


		virtual bool	IsValid() override								{ return myPlayerAction != PA_Max; }


	private :

		// ------ Private Variables ------

		PlayerAction	myPlayerAction;
};


#endif // !MOVE_EVENT_MESSAGE_H
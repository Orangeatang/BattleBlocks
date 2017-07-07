#ifndef MOUSE_MOVE_MESSAGE_H
#define MOUSE_MOVE_MESSAGE_H


// -------------------- Includes --------------------

#include "VEComponentMessage.h"

#include "Types.h"


// -------------------- Classes ---------------------

// A message used to send mouse move events to interested components
class MouseMoveMessage : public VEComponentMessage
{
	public :

		// ------- Public Enums -------

		enum MouseMoveAxis
		{
			MX_Pitch,
			MX_Yaw
		};


		// ----- Public Functions -----

		// Construction
		MouseMoveMessage( MouseMoveAxis anAxis, float aDelta ) : VEComponentMessage( MT_MouseMove ),
			myAxis( anAxis ),
			myDelta( aDelta )
		{
		}


		// Whether the movement is valid
		bool IsValid() override 
		{
			return myDelta != 0.0f; 
		}


		// ------- Accessors -------

		MouseMoveAxis	GetAxis()	{ return myAxis; }

		float			GetDelta()	{ return myDelta; }


	private :

		MouseMoveAxis	myAxis;
		float			myDelta;
};


#endif // !MOUSE_MOVE_MESSAGE_H
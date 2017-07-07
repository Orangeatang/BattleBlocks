#ifndef TYPES_H
#define TYPES_H


// ------------------------- Includes -------------------------

#include "VETypes.h"


// -------------------------- Enums ---------------------------

// Battle blocks specific components
enum GameComponent
{
	GC_PlayerInput = CM_Max,
	GC_PlayerPhysics,
	GC_Camera,
	
	GC_Max
};


// Various types of actions that can be performed by the player, used for key mapping
enum PlayerAction
{
	PA_MoveForward,
	PA_MoveBack,
	PA_StrafeLeft,
	PA_StrafeRight,
	PA_Sprint,
	PA_Walk,
	PA_Jump,

	PA_Max
};


// Various types of component messages
enum MessageType
{
	MT_BeginMovement,
	MT_EndMovement,
	MT_MouseMove,

	MT_Max
};


#endif // !TYPES_H
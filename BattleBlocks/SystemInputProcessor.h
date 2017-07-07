#ifndef INPUT_PROCESSOR_H
#define INPUT_PROCESSOR_H

// ------------------- Includes -------------------

#include <VETypes.h>


// ------------------- Classes --------------------

// Hands off keyboard/mouse input from the engine to various game components
class SystemInputProcessor
{
	public :

		// ---------- Public Functions ----------

		// Construction
		SystemInputProcessor();

		// Updates the game/engine state based on mouse/keyboard input
		void Update( float anElapsedTime );


	private :

		// ---------- Private Functions ---------

		// Changes the style of the chunk being rendered
		void SetChunkStyle( ChunkStyle aStyle );


		// ---------- Private Variables ---------

		float myMovementSpeed;
		float myFastMovementSpeed;
		float mySlowMovementSpeed;
};


#endif // !INPUT_PROCESSOR_H
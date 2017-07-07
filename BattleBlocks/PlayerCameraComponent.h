#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H


// ------------------------- Includes -------------------------

#include "VEObjectComponent.h"
#include "VETypes.h"


// -------------------- Forward Declarations ------------------

class VEBasicCamera;
class MouseMoveMessage;


// ------------------------- Classes --------------------------

// Processes movement messages from the physics component to move the player's camera. Also controls
// the type of camera used
class PlayerCameraComponent : public VEObjectComponent
{
	public :

		// ----- Public Functions -----

		// Construction
		PlayerCameraComponent( VEObject* aParent );

		// Sets the current camera for the player & engine
		void			SetCamera( CameraType aCameraType );


		// ---- Required Functions ----

		// Initialises the camera
		virtual bool	Initialise() override;

		// Cleans up the memory used by the camera
		virtual void	Cleanup() override;


	protected :

		// ---- Required Functions ----

		// Processes messages in the queue
		virtual void	ProcessMessages( float anElapsedTime ) override;


	private :

		// ---- Private Functions -----

		// Applies data from the move message to the active camera
		void			HandleMouseMoveMessage( MouseMoveMessage* aMoveMessage, float anElapsedTime );


		// ---- Private Variables -----

		VEBasicCamera* myCamera;
};


#endif // !CAMERA_COMPONENT_H
#ifndef VE_OBJECT_COMPONENT
#define VE_OBJECT_COMPONENT


// --------------------- Includes --------------------

#include "VETypes.h"
#include "VEComponentMessage.h"
#include "VESmartPointer.h"


// --------------- Forward Declarations --------------

class VEObject;


// --------------------- Classes ---------------------

// An object component is a compact unit of code that implements specific behaviors for engine objects e.g. physics, a.i.
// This is an abstract class, all children must implement the ProcessMessages function
class VEObjectComponent
{
	public :

		// ------ Public Functions ------

		// Construction
		VEObjectComponent( unsigned int aComponentType, VEObject* aParent );

		// Deconstruction
		virtual ~VEObjectComponent()												{}


		// Adds a component as a target for receiving messages
		void			AddMessageTarget( VEObjectComponent* aComponent );

		// Removes a component as a target for receiving messages
		void			RemoveMessageTarget( VEObjectComponent* aComponent );

		// Updates the state of the component
		virtual void	Update( float anElapsedTime );

		// Adds an object message to the component's message queue
		virtual bool	QueueMessage( VESmartPointer<VEComponentMessage> aMessage );


		// ----- Required Functions -----

		// Initialises the component
		virtual bool	Initialise()	= 0;

		// Cleans up any memory initialised by the component
		virtual void	Cleanup()		= 0;


		// --------- Accessors ----------

		unsigned int	GetType()						{ return myType; }

		bool			GetEnabled()					{ return myEnabled; }
		void			SetEnabled( bool anEnabled )	{ myEnabled = anEnabled; }

		const VEObject*	GetParent()						{ return myParent; }


	protected :		
		
		// ----- Protected Functions -----
		
		// Processes messages in the queue
		virtual void	ProcessMessages( float anElapsedTime ) = 0;

		// Sends a component message to all of the message targets
		void			SendMessage( VESmartPointer<VEComponentMessage> aMessage );

		// Adds a supported message type
		void			AddMessageType( unsigned int aMessageType );

		// Removes a supported message type
		void			RemoveMessageType( unsigned int aMessageType );
		

		// ----- Protected Variables -----

		bool											myEnabled;		

		unsigned int									myType;
		VEObject*										myParent;

		std::vector<VEObjectComponent*>					myMessageTargets;
		std::vector<VESmartPointer<VEComponentMessage>>	myMessageQueue;
		std::vector<unsigned int>						mySupportedMessageTypes;
};


#endif // !VE_OBJECT_COMPONENT
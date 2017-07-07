#ifndef VE_OBJECT_H
#define VE_OBJECT_H


// --------------------- Includes ----------------------

#include "VETypes.h"


// ---------------------- Defines ----------------------

#define VE_INVALID_OBJECT_ID -1


// ---------------- Forward Declarations ---------------

class VEObjectComponent;


// ---------------------- Classes ----------------------

// The base class from which all other engine objects are derived. Objects are made up from smaller code blocks,
// components, that define specific behaviors such as AI or Physics
class VEObject
{
	public :

		// ------ Public Functions ------

		// Construction
		VEObject();

		// Deconstruction
		virtual ~VEObject();		
		
		// Registers the object with the object manager
		virtual bool				Initialise( bool isDynamic = false );

		// Adds a component to the object
		bool						AddComponent( VEObjectComponent* aComponent );

		// Removes a component from the object
		void						RemoveComponent( ComponentType aComponentId );

		// Return the component of the specified type
		VEObjectComponent*			GetComponent( ComponentType aComponentId );


		// Updates the object and all of it's components
		virtual void				Update( float anElapsedTime );


		// --------- Accessors ----------

		int							GetId()												{ return myId; }
		void						SetId( int anId )									{ myId = anId; }

		const DirectX::XMFLOAT3&	GetPosition()										{ return myPosition; }
		void						SetPosition( const DirectX::XMFLOAT3& aPosition )	{ myPosition = aPosition; }

		const DirectX::XMFLOAT3&	GetRotation()										{ return myRotation; }
		void						SetRotation( const DirectX::XMFLOAT3& aRotation )	{ myRotation = aRotation; }

		const DirectX::XMFLOAT3		GetScale()											{ return myScale; }
		void						SetScale( const DirectX::XMFLOAT3& aScale )			{ myScale = aScale; }

		
		// ---- Overloaded Operators ----

		// Equality
		bool operator==( const VEObject* anObject );


	protected :

		// ----- Protected Variables -----

		DirectX::XMFLOAT3							myPosition;
		DirectX::XMFLOAT3							myRotation;
		DirectX::XMFLOAT3							myScale;

		int											myId;

		std::map<unsigned int, VEObjectComponent*>	myComponents;
};


#endif
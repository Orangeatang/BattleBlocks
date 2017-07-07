#ifndef VE_PHYSICS_OBJECT_H
#define VE_PHYSICS_OBJECT_H

// ---------------------- Includes ---------------------

#include "VEObjectComponent.h"
#include "VETypes.h"


// ---------------------- Classes ----------------------

// Defines the properties of an object that is affected by world physics. This is an abstract class that should
// used by child components for processing input messages
class VEPhysicsComponent : public VEObjectComponent
{
	public :

		// ------ Public Functions ------

		// Construction
		VEPhysicsComponent( VEObject* aParent, unsigned int aComponentType = CM_Physics );

		// Applies gravity to the object, then processes any incoming messages
		virtual void				Update( float anElapsedTime ) override;


		// ----- Required Functions -----

		// Registers the physics component with the physics service
		virtual bool				Initialise() override;

		// Unregisters the physics component with the physics service
		virtual void				Cleanup() override;


		// --------- Accessors ----------

		const DirectX::XMFLOAT3		GetVelocity()												{ return myVelocity; }
		void						SetVelocity( const DirectX::XMFLOAT3& aVelocity )			{ myVelocity = aVelocity; }

		const DirectX::XMFLOAT3		GetAcceleration()											{ return myAcceleration; }
		void						SetAcceleration( const DirectX::XMFLOAT3& anAcceleration )	{ myAcceleration = anAcceleration; }

		float						GetMass()													{ return myMass; }
		void						SetMass( float aMass )										{ myMass = aMass; }

		bool						IsFalling()													{ return myVelocity.y < 0.0f; }
		bool						IsJumping()													{ return myVelocity.y > 0.0f; }
		bool						IsInAir()													{ return IsFalling() || IsJumping(); }


	protected :	

		// ---- Protected Functions -----

		// Applies gravity to the velocity of the object
		virtual void				ApplyGravity( float anElapsedTime );


		// ---- Protected Variables -----

		DirectX::XMFLOAT3	myVelocity;
		DirectX::XMFLOAT3	myAcceleration;
		
		float				myMass;
};


#endif // !VE_PHYSICS_OBJECT_H
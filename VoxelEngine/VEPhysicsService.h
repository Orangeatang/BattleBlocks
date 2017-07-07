#ifndef VE_PHYSICS_SERVICE_H
#define VE_PHYSICS_SERVICE_H


// ---------------- Forward Declarations ---------------

class VEObject;
class VEChunk;


// ---------------------- Classes ----------------------

// A service used to update the position and orientation of all registered physics objects. Maintains
// physics related constants (gravity etc)
class VEPhysicsService
{
	public :

		// ------ Public Functions ------

		// Construction
		VEPhysicsService();

		// Updates the position & orientation of all registered physics objects
		void	Update( float anElapsedTime );

		// Registers an object for physics updates
		void	RegisterObject( VEObject* anObject );

		// Unregisters an object from physics updates
		void	UnregisterObject( int anObjectId );

		// Validates whether an entity can move from it's current position to the target position by checking
		// for collisions with nearby voxels and entities
		bool	ValidateMovement( const DirectX::XMFLOAT3& aCurrentPosition, DirectX::XMFLOAT3& aTargetPosition );


		// --------- Accessors ----------

		bool	GetEnabled()					{ return myEnabled; }

		void	SetEnabled( bool anEnabled )	{ myEnabled = anEnabled; }

		// The force due to gravity (in voxels per second)
		float	GetGravity()					{ return myGravity; }
		void	SetGravity( float aGravity )	{ myGravity = aGravity; }


	private :

		// ------ Private Variables -----

		bool					myEnabled;
		std::vector<VEObject*>	myPhysicsObjects;

		float					myGravity;
};


#endif // !VE_PHYSICS_SERVICE_H
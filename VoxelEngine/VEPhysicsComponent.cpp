
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VEPhysicsComponent.h"

#include "VoxelEngine.h"
#include "VEPhysicsService.h"
#include "VEObject.h"
#include "VEChunkManager.h"
#include "VEChunk.h"
#include "VEVoxel.h"


// --------------------- Namespaces ---------------------

using namespace DirectX;


// ------------------ Class Functions -------------------

// Construction
VEPhysicsComponent::VEPhysicsComponent( VEObject* aParent, unsigned int aComponentType /* = CM_Physics */ ) :
	VEObjectComponent( aComponentType, aParent ),
	myVelocity( 0.0f, 0.0f, 0.0f ),
	myAcceleration( 0.0f, 0.0f, 0.0f ),
	myMass( 1.0f )
{
}


// Applies gravity to the object, then processes any incoming messages
void VEPhysicsComponent::Update( float anElapsedTime )
{
	ApplyGravity( anElapsedTime );
	VEObjectComponent::Update( anElapsedTime );
}


// Registers the physics component with the physics service
bool VEPhysicsComponent::Initialise()
{
 	VEPhysicsService* physicsService = VoxelEngine::GetInstance()->GetPhysicsService();
 	assert( physicsService != NULL );

	physicsService->RegisterObject( myParent );

	return true;
}


// Unregisters the physics component with the physics service
void VEPhysicsComponent::Cleanup()
{
 	VEPhysicsService* physicsService = VoxelEngine::GetInstance()->GetPhysicsService();
	if( physicsService != NULL )
	{
		physicsService->UnregisterObject( myParent->GetId() );
	}
}


// Applies gravity to the velocity of the object
void VEPhysicsComponent::ApplyGravity( float anElapsedTime )
{
	VEPhysicsService* physicsService = VoxelEngine::GetInstance()->GetPhysicsService();
	assert( physicsService != NULL );

	float gravity = physicsService->GetGravity();

	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEChunk* activeChunk = chunkManager->GetChunk( myParent->GetPosition() );
	if( activeChunk )
	{
		XMINT3 chunkOffset; 
		chunkManager->CalculateVoxelOffset( chunkOffset, myParent->GetPosition(), activeChunk );
		if( VEVoxel* lowerVoxel = activeChunk->GetVoxel(chunkOffset.x, chunkOffset.y - 1, chunkOffset.z) )
		{
			if( lowerVoxel->GetEnabled() )
			{
				myVelocity.y = 0.0f;
				return;
			}
		}
	}

	myVelocity.y += physicsService->GetGravity() * anElapsedTime;
}
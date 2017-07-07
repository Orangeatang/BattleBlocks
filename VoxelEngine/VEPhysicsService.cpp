
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VEPhysicsService.h"

#include "VoxelEngine.h"
#include "VEObject.h"
#include "VEPhysicsComponent.h"
#include "VEChunkManager.h"
#include "VEChunk.h"
#include "VEVoxel.h"


// ---------------------- Namespaces ---------------------

using namespace DirectX;


// ------------------- Class Functions -------------------

// Construction
VEPhysicsService::VEPhysicsService() :
	myEnabled( false ),
	myGravity( -10.0f )
{
}


// Updates the position & orientation of all registered physics objects
void VEPhysicsService::Update( float anElapsedTime )
{
}


// Registers an object for physics updates
void VEPhysicsService::RegisterObject( VEObject* anObject )
{
	if( std::find(myPhysicsObjects.begin(), myPhysicsObjects.end(), anObject) == myPhysicsObjects.end() )
	{
		myPhysicsObjects.push_back( anObject );
	}
}


// Unregisters an object from physics updates
void VEPhysicsService::UnregisterObject( int anObjectId )
{
	for( unsigned int i = 0; i < myPhysicsObjects.size(); i++ )
	{
		if( anObjectId == myPhysicsObjects[i]->GetId() )
		{
			myPhysicsObjects.erase( myPhysicsObjects.begin() + i );
			return;
		}
	}
}


// Validates whether an entity can move from it's current position to the target position by checking
// for collisions with nearby voxels and entities
bool VEPhysicsService::ValidateMovement( const DirectX::XMFLOAT3& aCurrentPosition, DirectX::XMFLOAT3& aTargetPosition )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );

	VEChunk* activeChunk = chunkManager->GetChunk( aCurrentPosition );
	if( activeChunk != NULL )
	{
		XMINT3 requestedOffset;
		chunkManager->CalculateVoxelOffset( requestedOffset, aTargetPosition, activeChunk );
		if( VEVoxel* targetVoxel = activeChunk->GetVoxel(requestedOffset.x, requestedOffset.y, requestedOffset.z) )
		{
			if( targetVoxel->GetEnabled() )
			{
				return false;
			}
		}
	}

	return true;
}
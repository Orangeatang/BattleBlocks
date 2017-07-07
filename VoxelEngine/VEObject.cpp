
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VEObject.h"

#include "VoxelEngine.h"
#include "VEObjectComponent.h"
#include "VEObjectService.h"


// --------------------- Namespaces ---------------------

using namespace DirectX;


// ---------------------- Typedefs ----------------------

typedef std::pair<unsigned int, VEObjectComponent*>				ComponentEntry;
typedef std::map<unsigned int, VEObjectComponent*>::iterator	ComponentIter;


// ------------------ Class Functions -------------------

// Construction
VEObject::VEObject() :
	myId( VE_INVALID_OBJECT_ID ),
	myPosition( XMFLOAT3(0.0f, 0.0f, 0.0f) ),
	myRotation( XMFLOAT3(0.0f, 0.0f, 0.0f) ),
	myScale( XMFLOAT3(1.0f, 1.0f, 1.0f) )
{
}


// Deconstruction
VEObject::~VEObject()
{
	for( ComponentIter iter = myComponents.begin(); iter != myComponents.end(); iter++ )
	{
		iter->second->Cleanup();

		delete iter->second;
		iter->second = NULL;
	}

	myComponents.clear();
}


// Registers the object with the object manager
bool VEObject::Initialise( bool isDynamic /* = false */ )
{
	VEObjectService* objectService = VoxelEngine::GetInstance()->GetObjectService();
	assert( objectService != NULL );

	objectService->RegisterObject( this, isDynamic );

	return true;
}


// Adds a component to the object
bool VEObject::AddComponent( VEObjectComponent* aComponent )
{
	ComponentIter component = myComponents.find( aComponent->GetType() );
	if( component == myComponents.end() )
	{
		myComponents.insert( ComponentEntry(aComponent->GetType(), aComponent) );
		return true;
	}

	return false;
}


// Removes a component from the object
void VEObject::RemoveComponent( ComponentType aComponentId )
{
	ComponentIter component = myComponents.find( aComponentId );
	if( component != myComponents.end() )
	{
		delete myComponents[aComponentId];
		myComponents[aComponentId] = NULL;

		myComponents.erase( component );
	}
}


// Return the component of the specified type
VEObjectComponent* VEObject::GetComponent( ComponentType aComponentId )
{
	if( myComponents.find(aComponentId) != myComponents.end() )
	{
		return myComponents[aComponentId];
	}

	return NULL;
}


// Updates the object and all of it's components
void VEObject::Update( float anElapsedTime )
{	
	for( ComponentIter component = myComponents.begin(); component != myComponents.end(); component++ )
	{
		(*component).second->Update( anElapsedTime );
	}
}


// Overloaded equality operator
bool VEObject::operator==( const VEObject* anObject ) 
{
	if( anObject->myId == myId )
	{
		return true;
	}

	return false;
}
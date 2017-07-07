
// -------------------- Includes ---------------------

#include "Stdafx.h"
#include "VEObjectService.h"

#include "VEObject.h"
#include "VEObjectComponent.h"
#include "VEPhysicsComponent.h"


// -------------------- Statics ----------------------

int VEObjectService::myNextObjectId = 0;


// -------------------- Typedefs ---------------------

typedef std::pair<int, VEObject*>			ObjectEntry;
typedef std::map<int, VEObject*>::iterator	ObjectIter;


// ----------------- Class Functions -----------------

// Construction
VEObjectService::VEObjectService()
{
}


// Deconstruction
VEObjectService::~VEObjectService()
{
	for( ObjectIter iter = myStaticObjects.begin(); iter != myStaticObjects.end(); iter++ )
	{
		delete iter->second;
		iter->second = NULL;
	}

	myStaticObjects.clear();
}


// Registers an object with the manager
bool VEObjectService::RegisterObject( VEObject* anObject, bool isDynamic )
{
	if( anObject->GetId() == VE_INVALID_OBJECT_ID )
	{
		anObject->SetId( GetNextId() );
	}

	if( isDynamic )
	{
		if( myDynamicObjects.find(anObject->GetId()) == myDynamicObjects.end() )
		{
			myDynamicObjects.insert( ObjectEntry(anObject->GetId(), anObject) );
			return true;
		}
	}
	else
	{
		if( myStaticObjects.find(anObject->GetId()) == myStaticObjects.end() )
		{
			myStaticObjects.insert( ObjectEntry(anObject->GetId(), anObject) );
			return true;
		}
	}


	return false;
}


// Returns the object with the supplied id
VEObject* VEObjectService::GetObject( int anObjectId )
{
	if( anObjectId == VE_INVALID_OBJECT_ID )
	{
		return NULL;
	}
	
	ObjectIter object = myDynamicObjects.find( anObjectId );
	if( object != myDynamicObjects.end() )
	{
		return object->second;
	}
	
	object = myStaticObjects.find( anObjectId );
	if( object != myStaticObjects.end() )
	{
		return object->second;
	}

	return NULL;
}


// Updates all of the registered dynamic objects
void VEObjectService::Update( float anElapsedTime )
{
	for( ObjectIter object = myDynamicObjects.begin(); object != myDynamicObjects.end(); object++ )
	{
		object->second->Update( anElapsedTime );
	}
}


// Returns the next object id, increments the counter
int VEObjectService::GetNextId()
{
	return myNextObjectId++;
}
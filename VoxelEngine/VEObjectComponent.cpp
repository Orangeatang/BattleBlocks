
// --------------------- Includes ---------------------

#include "Stdafx.h"
#include "VEObjectComponent.h"

#include "VEObject.h"
#include "VEComponentMessage.h"


// --------------------- Typedefs ---------------------

typedef std::vector<VEComponentMessage*>::iterator MessageIterator;


// ----------------- Class Functions ------------------

// Construction
VEObjectComponent::VEObjectComponent( unsigned int aComponentType, VEObject* aParent ) :
	myType( aComponentType ),
	myParent( aParent ),
	myEnabled( true )
{
}


// Adds a component as a target for receiving messages
void VEObjectComponent::AddMessageTarget( VEObjectComponent* aComponent )
{
	assert( aComponent != NULL );

	// Make sure we don't have the component already
	for( std::vector<VEObjectComponent*>::const_iterator iter = myMessageTargets.begin(); iter != myMessageTargets.end(); iter++ )
	{
		if( *iter == aComponent )
		{
			return;
		}
	}

	myMessageTargets.push_back( aComponent );
}


// Removes a component as a target for receiving messages
void VEObjectComponent::RemoveMessageTarget( VEObjectComponent* aComponent )
{
	assert( aComponent != NULL );

	// Remove the component
	for( std::vector<VEObjectComponent*>::iterator iter = myMessageTargets.begin(); iter != myMessageTargets.end(); iter++ )
	{
		if( *iter == aComponent )
		{
			myMessageTargets.erase( iter );
			*iter = NULL;

			return;
		}
	}
}


// Updates the state of the component
void VEObjectComponent::Update( float anElapsedTime )
{
	if( myMessageQueue.size() > 0 )
	{
		ProcessMessages( anElapsedTime );
	}
}


// Adds a message to the queue
bool VEObjectComponent::QueueMessage( VESmartPointer<VEComponentMessage> aMessage )
{
	if( std::find(mySupportedMessageTypes.begin(), mySupportedMessageTypes.end(), aMessage->GetType()) != mySupportedMessageTypes.end() )
	{
		myMessageQueue.push_back( aMessage );
		return true;
	}
	
	return false;
}


// Sends a component message to all of the message targets
void VEObjectComponent::SendMessage( VESmartPointer<VEComponentMessage> aMessage )
{
	for( std::vector<VEObjectComponent*>::iterator iter = myMessageTargets.begin(); iter != myMessageTargets.end(); iter++ )
	{
		(*iter)->QueueMessage( aMessage );
	}
}


// Adds a supported message type
void VEObjectComponent::AddMessageType( unsigned int aMessageType )
{
	if( std::find(mySupportedMessageTypes.begin(), mySupportedMessageTypes.end(), aMessageType) == mySupportedMessageTypes.end() )
	{
		mySupportedMessageTypes.push_back( aMessageType );
	}
}


// Removes a supported message type
void VEObjectComponent::RemoveMessageType( unsigned int aMessageType )
{
	std::vector<unsigned int>::iterator iter = std::find(mySupportedMessageTypes.begin(), mySupportedMessageTypes.end(), aMessageType );
	if( iter != mySupportedMessageTypes.end() )
	{
		mySupportedMessageTypes.erase( iter );
	}
}
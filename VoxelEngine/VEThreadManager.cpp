
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VEThreadManager.h"

#include "VoxelEngine.h"


// ------------------- Class Functions ------------------

// Construction
VEThreadManager::VEThreadManager() :
	myMaxThreadCount( 0 ),
	myCurrentTheadId( 0 )
{
}


// Creates the thread pool
bool VEThreadManager::Initialise( unsigned int aMaxThreadCount /* = 15 */)
{
	myMaxThreadCount = aMaxThreadCount;

	return true;
}


// Stops all active threads and frees up the memory used by the thread manager
void VEThreadManager::Uninitialise()
{
	for( unsigned int i = 0; i < myPendingThreads.size(); i++ )
	{
		delete myPendingThreads[i];
		myPendingThreads[i] = NULL;
	}
	myPendingThreads.clear();

	DWORD exitCode;
	for( unsigned int i = 0; i < myActiveThreads.size(); i++ )
	{
		GetExitCodeThread(myActiveThreads[i]->myThread, &exitCode);
		if( exitCode == STILL_ACTIVE )
		{
			TerminateThread( myActiveThreads[i]->myThread, myActiveThreads[i]->myThreadId );
		}

		delete myActiveThreads[i];
		myActiveThreads[i] = NULL;
	}
	myActiveThreads.clear();
}


// Adds a job to the pending thread list
bool VEThreadManager::SpawnThread( LPTHREAD_START_ROUTINE aThreadFunction, LPVOID aThreadParameter, VE_THREAD_CALLBACK aCallback /* = NULL */, LPVOID aCallbackParameter /* = NULL */ )
{
	if( aThreadFunction == NULL )
	{
		return false;
	}

	myPendingThreads.push_back( new PendingThread(aThreadFunction, aThreadParameter, aCallback, aCallbackParameter) );

	return true;
}

// Stops all active threads and frees up the memory used by the thread manager
void VEThreadManager::Update( float anElapsedTime )
{
	UpdateActiveThreads();

	UpdatePendingThreads();
}


// Checks the status of all active threads, removing completed ones from the queue
void VEThreadManager::UpdateActiveThreads()
{
	// Remove any threads that have finished executing
	DWORD exitCode;
	for( unsigned int i = 0; i < myActiveThreads.size(); )
	{
		if( GetExitCodeThread(myActiveThreads[i]->myThread, &exitCode) )
		{
			if( exitCode != STILL_ACTIVE )
			{
				// Execute the callback
				if( myActiveThreads[i]->myCallback != NULL )
				{
					myActiveThreads[i]->myCallback( myActiveThreads[i]->myCallbackParameter );
				}

				delete myActiveThreads[i];
				myActiveThreads[i] = NULL;
				myActiveThreads.erase( myActiveThreads.begin() + i );

				continue;
			}
		}

		i++;
	}
}


// Adds any new pending threads to the active thread pool
void VEThreadManager::UpdatePendingThreads()
{
	for( unsigned int i = 0; i < myPendingThreads.size(); )
	{
		if( myActiveThreads.size() < (unsigned int)myMaxThreadCount )
		{
			DWORD	threadId		= 0;
			HANDLE	threadHandle	= CreateThread( NULL, 0, myPendingThreads[i]->myThreadFunction, myPendingThreads[i]->myThreadParameter, 0, &threadId );
			assert( threadHandle != NULL );

			myActiveThreads.push_back( new ActiveThread(threadHandle, threadId, myPendingThreads[i]->myCallback, myPendingThreads[i]->myCallbackParameter) );

			delete myPendingThreads[i];
			myPendingThreads[i] = NULL;
			myPendingThreads.erase( myPendingThreads.begin() + i );

			continue;
		}

		i++;
	}
}
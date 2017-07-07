#ifndef VE_THREAD_MANAGER_H
#define VE_THREAD_MANAGER_H


// ---------------------- Typedefs ---------------------

typedef void (*VE_THREAD_CALLBACK)( LPVOID aParameter );
typedef long int VEThreadId;


// ---------------------- Classes ----------------------

// Manages a thread pool, checks active thread status each frame, executes 'on complete' callbacks
class VEThreadManager
{
	public :

		// --------- Public Functions ---------

		// Construction
		VEThreadManager();

		// Creates the thread pool
		bool Initialise( unsigned int aMaxTheadCount = 15 );

		// Stops all active threads and frees up the memory used by the thread manager
		void Uninitialise();

		// Adds a job to the pending thread list
		bool SpawnThread( LPTHREAD_START_ROUTINE aThreadFunction, LPVOID aThreadParameter, VE_THREAD_CALLBACK aCallback = NULL, LPVOID aCallbackParameter = NULL );

		// Updates the active threads
		void Update( float anElapsedTime );


	private :

		// --------- Private Structures -------

		// Threads waiting to execute
		struct PendingThread
		{
			// Construction
			PendingThread( LPTHREAD_START_ROUTINE aThreadFunction, LPVOID aThreadParameter, VE_THREAD_CALLBACK aCallback, LPVOID aCallbackParameter )
			{
				myThreadFunction	= aThreadFunction;
				myThreadParameter	= aThreadParameter;
				
				myCallback			= aCallback;
				myCallbackParameter	= aCallbackParameter;
			}

			// Deconstruction
			~PendingThread()
			{
				myThreadFunction	= NULL;
				myThreadParameter	= NULL;
				
				myCallback			= NULL;
				myCallbackParameter	= NULL;
			}

			VE_THREAD_CALLBACK		myCallback;
			LPVOID					myCallbackParameter;

			LPTHREAD_START_ROUTINE	myThreadFunction;
			LPVOID					myThreadParameter;
		};

		// Currently active threads
		struct ActiveThread
		{
			// Construction
			ActiveThread( HANDLE aThread, VEThreadId anId, VE_THREAD_CALLBACK aCallback, LPVOID aCallbackParameter )
			{
				myThread			= aThread;
				myThreadId			= anId;

				myCallback			= aCallback;
				myCallbackParameter = aCallbackParameter;
			}

			// Deconstruction
			~ActiveThread()
			{
				myThread			= NULL;
				myThreadId			= 0;
				
				myCallback			= NULL;
				myCallbackParameter	= NULL;
			}

			VE_THREAD_CALLBACK	myCallback;
			LPVOID				myCallbackParameter;

			HANDLE				myThread;
			VEThreadId			myThreadId;
		};


		// --------- Private Functions --------

		// Checks the status of all active threads, removing completed ones from the queue
		void					UpdateActiveThreads();

		// Adds any new pending threads to the active thread pool
		void					UpdatePendingThreads();


		// --------- Private Variables --------

		std::vector<PendingThread*> myPendingThreads;
		std::vector<ActiveThread*>	myActiveThreads;

		unsigned int				myMaxThreadCount;

		long int					myCurrentTheadId;
};


#endif // !_VE_THREAD_MANAGER_H_
#ifndef VE_SMART_POINTER_H
#define VE_SMART_POINTER_H


// ------------------- Structs -------------------


// ------------------- Classes -------------------

// Used by the smart pointer class to keep track of the number of references to a particular 
// block of data
class VEReferenceCount
{
	public :	

		// ---- Public Functions -----

		// Construction
		VEReferenceCount()
		{
			myReferenceCount = 0;
		}

		// Adds a reference
		void AddReference()
		{
			myReferenceCount++;
		}

		// Releases a reference
		int Release()
		{
			myReferenceCount--;
			assert( myReferenceCount >= 0 );

			return myReferenceCount;
		}


	private :

		// ---- Private Variables ----

		int myReferenceCount;
};


// A class that keeps track of the number of references to a block of data. If the references to that data
// reach zero, the data is released
template <typename T>
class VESmartPointer
{
	public :

		// ---- Private Functions ----

		// Default constructor
		VESmartPointer() :
			myData( NULL ),
			myReferenceCount( NULL )
		{
			myReferenceCount = new VEReferenceCount();
			myReferenceCount->AddReference();
		}

		// Data constructor. Creates a new reference counter for the supplied data
		VESmartPointer( T* someData ) :
			myData( someData ),
			myReferenceCount( NULL )
		{
			myReferenceCount = new VEReferenceCount();
			myReferenceCount->AddReference();
		}

		// Copy constructor. Copies the data and the reference count. Increments the reference count.
		VESmartPointer( const VESmartPointer<T>& aSmartPointer ) :
			myData( aSmartPointer.myData ),
			myReferenceCount( aSmartPointer.myReferenceCount )
		{
			myReferenceCount->AddReference();
		}

		// Destructor. Decrements the reference count. Releases the data if the reference count is 0
		~VESmartPointer()
		{
			if( myReferenceCount->Release() == 0 )
			{
				delete myData;
				delete myReferenceCount;

				myData				= NULL;
				myReferenceCount	= NULL;
			}
		}


		// --- Overridden Operators --

		// Returns the de-referenced data
		virtual T& operator* ()
		{
			return *myData;
		}

		// Returns the data
		virtual T* operator& ()
		{
			return myData;
		}

		// Returns a pointer to the data
		virtual T* operator-> ()
		{
			return myData;
		}

		// Assignment operator. Decrements the old reference counter if the new data is different
		virtual VESmartPointer<T>& operator= ( const VESmartPointer<T>& aSmartPointer )
		{
			// Avoid self incrementation of the reference count
			if( this != &aSmartPointer )
			{
				if( myReferenceCount->Release() == 0 )
				{
					delete myData;
					delete myReferenceCount;

					myData				= NULL;
					myReferenceCount	= NULL;
				}

				// Copy the data & reference pointer. Increment the reference count
				myData				= aSmartPointer.myData;
				myReferenceCount	= aSmartPointer.myReferenceCount;
				myReferenceCount->AddReference();
			}

			return *this;
		}


	private :

		// ---- Private Variables ----

		VEReferenceCount*	myReferenceCount;
		T*					myData;
};

#endif // !VE_SMART_POINTER_H
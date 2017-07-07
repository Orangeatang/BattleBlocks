#ifndef VE_COMPONENT_MESSAGE_H
#define VE_COMPONENT_MESSAGE_H


// ------------------------- Classes -------------------------

// A base class used for communication between components
class VEComponentMessage
{
	public :

		// ----- Public Functions -----

		// Construction
		VEComponentMessage( unsigned int aMesageType );


		// -------- Accessors ---------

		unsigned	int		GetType()	{ return myType; }

		virtual		bool	IsValid()	{ return true; }


	protected :

		unsigned int myType;
};


#endif // !VE_COMPONENT_MESSAGE_H
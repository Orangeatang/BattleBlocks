#ifndef VE_DIRECTX_INPUT_H
#define VE_DIRECTX_INPUT_H


// ----------------------- Classes -----------------------

// Handles the DirectX keyboard, mouse and gamepad interface. Input listeners can register
// themselves with this class, so they are informed when input events occur
class VEDirectXInput
{
	public :

		// -------- Public Functions --------

		// Construction
		VEDirectXInput();

		// Initialises the DirectInput interface
		bool Initialise( HINSTANCE anInstance, HWND aWindow, int aScreenWidth, int aScreenHeight );

		// Cleans up the memory used by the input interface
		void Uninitialise();

        // Checks the device states and sends updates to any registered listeners
        void Update();


		// ----------- Accessors -----------

		// Whether the supplied key is being pressed
		bool IsKeyDown( unsigned int aKey )	
		{ 
			if( myKeyboardState[aKey] & 0x80 )
			{
				return true;
			}
			
			return false;
		}

		// Difference between the current mouse-x and the previous mouse-x
		long MouseDeltaX()					{ return myMouseState.lX - myMouseState.lY; }

		// Difference between the current mouse-y and the previous mouse-y
		long MouseDeltaY()					{ return myMouseState.lY - myPreviousMouseState.lY; }
		
		// Current mouse-x
		long MouseX()						{ return myMouseState.lX; }
		
		// Current mouse-y
		long MouseY()						{ return myMouseState.lY; }


	private :

		// -------- Private Functions -------

		// Initialise the keyboard device
		bool InitialiseKeyboard( HWND aWindow );

		// Initialise the mouse device
		bool InitialiseMouse( HWND aWindow );

		// Initialise the gamepad device
		bool InitialiseGamepad();


		// -------- Private Variables -------

		IDirectInput8*			myDirectInput;
		IDirectInputDevice8*	myKeyboard;
		IDirectInputDevice8*	myMouse;

		unsigned char			myKeyboardState[256];
        unsigned char           myPreviousKeyboardState[256];
		
        DIMOUSESTATE			myMouseState;
        DIMOUSESTATE            myPreviousMouseState;

		int						myScreenWidth;
		int						myScreenHeight;
};


#endif // !VE_DIRECTX_INPUT_H
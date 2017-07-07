
// ----------------------- Includes -----------------------

#include "Stdafx.h"
#include "VEDirectXInput.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"


// ----------------------- Namespaces ---------------------

using namespace DirectX;


// -------------------- Class Functions -------------------

// Construction
VEDirectXInput::VEDirectXInput() :
	myDirectInput( NULL ),
	myMouse( NULL ),
	myKeyboard( NULL ),
	myScreenWidth( 1024 ),
	myScreenHeight( 768 )
{
    memset( &myKeyboardState, 0, sizeof(char) * 256 );
    memset( &myPreviousKeyboardState, 0, sizeof(char) * 256 );

    memset( &myMouseState, 0, sizeof(DIMOUSESTATE) );
    memset( &myPreviousMouseState, 0, sizeof(DIMOUSESTATE) );
}


// Initialises the DirectInput interface
bool VEDirectXInput::Initialise( HINSTANCE anInstance, HWND aWindow, int aScreenWidth, int aScreenHeight )
{
	myScreenWidth	= aScreenWidth;
	myScreenHeight	= aScreenHeight;

	HRESULT result;

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	// Initialise the direct input interface
	result = DirectInput8Create( anInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&myDirectInput, NULL );
	if( FAILED(result) )
	{
		return false;
	}

	if( !InitialiseKeyboard(aWindow) )
	{
		return false;
	}

	if( !InitialiseMouse(aWindow) )
	{
		return false;
	}

	if( !InitialiseGamepad() )
	{
		return false;
	}

	return true;
}


// Cleans up the memory used by the input interface
void VEDirectXInput::Uninitialise()
{
	if( myKeyboard != NULL )
	{
		myKeyboard->Release();
		myKeyboard = NULL;
	}

	if( myMouse != NULL )
	{
		myMouse->Release();
		myMouse = NULL;
	}

	if( myDirectInput != NULL )
	{
		myDirectInput->Release();
		myDirectInput = NULL;
	}
}


// Checks the device states and sends updates to any registered listeners
void VEDirectXInput::Update()
{
    HRESULT result;

    // Read in the keyboard state
    memcpy( &myPreviousKeyboardState, &myKeyboardState, sizeof(char) * 256 );
    result = myKeyboard->GetDeviceState( sizeof(myKeyboardState), (LPVOID)&myKeyboardState );
    
    // Reacquire the keyboard if needed
    if( FAILED(result) )
    {
        if( result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED )
        {
            myKeyboard->Acquire();
        }
    }

    // Read in the mouse state
    myPreviousMouseState = myMouseState;
    result = myMouse->GetDeviceState( sizeof(DIMOUSESTATE), (LPVOID)&myMouseState );
    
    // Reacquire the mouse if needed
    if( FAILED(result) )
    {
        if( result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED )
        {
            myMouse->Acquire();
        }
    }
}


// Initialise the keyboard device
bool VEDirectXInput::InitialiseKeyboard( HWND aWindow )
{
	HRESULT result;

	// Create the keyboard interface
	result = myDirectInput->CreateDevice( GUID_SysKeyboard, &myKeyboard, NULL );
	if( FAILED(result) )
	{
		return false;
	}

	// Set the keyboard data format
	result = myKeyboard->SetDataFormat( &c_dfDIKeyboard );
	if( FAILED(result) )
	{
		return false;
	}

	// Set the keyboard cooperative level
	result = myKeyboard->SetCooperativeLevel( aWindow, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND );
	if( FAILED(result) )
	{
		return false;
	}

	// Finally, acquire the keyboard
	result = myKeyboard->Acquire();
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Initialise the mouse device
bool VEDirectXInput::InitialiseMouse( HWND aWindow )
{
	HRESULT result;

	// Create the mouse interface
	result = myDirectInput->CreateDevice( GUID_SysMouse, &myMouse, NULL );
	if( FAILED(result) )
	{
		return false;
	}

	// Set the data format for the mouse
	result = myMouse->SetDataFormat( &c_dfDIMouse );
	if( FAILED(result) )
	{
		return false;
	}

	// Set the mouse cooperative level
	result = myMouse->SetCooperativeLevel( aWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
	if( FAILED(result) )
	{
		return false;
	}

	// Finally, acquire the mouse
	result = myMouse->Acquire();
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Initialise the gamepad device
bool VEDirectXInput::InitialiseGamepad()
{
	return true;
}
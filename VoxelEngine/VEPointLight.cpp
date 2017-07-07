
// ------------------ Includes ------------------

#include "Stdafx.h"
#include "VEPointLight.h"


// ----------------- Namespaces -----------------

using namespace DirectX;


// --------------- Class Functions --------------

// Construction
VEPointLight::VEPointLight() : 
	VELight( LT_Point ),
	myIntensity( 1.0f ),
	myRadius( 5.0f )
{
}
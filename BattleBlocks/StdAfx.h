#ifndef STDAFX_H
#define STDAFX_H

// ------------------ Defines ------------------

#define _WIN32_WINNT		0x0600

#define DIRECTINPUT_VERSION 0x0800


// ------------------ Includes -----------------

// Windows
#include <windows.h>

// DirectX
#include <d3d11.h>
#include <DirectXMath.h>
#include <dinput.h>

// std lib
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#endif // !STDAFX_H
#ifndef STDAFX_H
#define STDAFX_H


// ------------------ Defines ------------------

#define _WIN32_WINNT		0x0600

#define DIRECTINPUT_VERSION 0x0800

#define ALIGN_16 _declspec(align(16))


// ------------------ Includes ------------------

// Windows
#include <wincodec.h>
#include <winbase.h>
#include <time.h>

// DirectX
#include <dxgi.h>
#include <D3DCommon.h>
#include <d3d11.h>
#include <dinput.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

// std lib
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

#endif // !STDAFX_H
#pragma once

//turn off stupid warnings
#define _CRT_SECURE_NO_WARNINGS

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include <iostream>
#include <fstream>
#include <functional>
#include <exception>
#include <algorithm>
using namespace std;

#define proprw(t, n, gc) inline t& n() gc
#define propr(t, n, gc) inline t n() const gc

//using GLM for GL and DX because I am lazy
#include <glm/glm.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_operation.hpp>
using namespace glm;

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef DIRECTX
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#ifdef LINK
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "xinput.lib")
#endif
#endif

#define OPENGL
#ifdef OPENGL
#include <GL/glew.h>
#include <GL/wglew.h>
#ifdef LINK
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#endif
#endif

namespace qeg
{
	typedef unsigned int uint;	
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
	typedef unsigned char byte;

	inline float randf() { return ((float)rand() / (float)RAND_MAX); }
	inline float randfn() { return ((randf() * 2) - 1); }

#ifdef DIRECTX
	//HRexception
	// exception that resulted from a failed HRESULT, which is passed along
	struct hresult_exception : public exception
	{
	public:
		HRESULT hr;
		hresult_exception(HRESULT h, const char* m = "") : hr(h), exception(m) { }
	};	
	//chr : check if FAILED(hr) and if so, throw a HRexception
	inline void chr(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw new hresult_exception(hr);
		}
	}

	enum class pixel_format
	{
		UNKNOWN = 0,
		R32G32B32A32_TYPELESS = 1,
		R32G32B32A32_FLOAT = 2,
		R32G32B32A32_UINT = 3,
		R32G32B32A32_SINT = 4,
		R32G32B32_TYPELESS = 5,
		R32G32B32_FLOAT = 6,
		R32G32B32_UINT = 7,
		R32G32B32_SINT = 8,
		R16G16B16A16_TYPELESS = 9,
		R16G16B16A16_FLOAT = 10,
		R16G16B16A16_UNORM = 11,
		R16G16B16A16_UINT = 12,
		R16G16B16A16_SNORM = 13,
		R16G16B16A16_SINT = 14,
		R32G32_TYPELESS = 15,
		R32G32_FLOAT = 16,
		R32G32_UINT = 17,
		R32G32_SINT = 18,
		R32G8X24_TYPELESS = 19,
		D32_FLOAT_S8X24_UINT = 20,
		R32_FLOAT_X8X24_TYPELESS = 21,
		X32_TYPELESS_G8X24_UINT = 22,
		R10G10B10A2_TYPELESS = 23,
		R10G10B10A2_UNORM = 24,
		R10G10B10A2_UINT = 25,
		R11G11B10_FLOAT = 26,
		R8G8B8A8_TYPELESS = 27,
		R8G8B8A8_UNORM = 28,
		R8G8B8A8_UNORM_SRGB = 29,
		R8G8B8A8_UINT = 30,
		R8G8B8A8_SNORM = 31,
		R8G8B8A8_SINT = 32,
		R16G16_TYPELESS = 33,
		R16G16_FLOAT = 34,
		R16G16_UNORM = 35,
		R16G16_UINT = 36,
		R16G16_SNORM = 37,
		R16G16_SINT = 38,
		R32_TYPELESS = 39,
		D32_FLOAT = 40,
		R32_FLOAT = 41,
		R32_UINT = 42,
		R32_SINT = 43,
		R24G8_TYPELESS = 44,
		D24_UNORM_S8_UINT = 45,
		R24_UNORM_X8_TYPELESS = 46,
		X24_TYPELESS_G8_UINT = 47,
		R8G8_TYPELESS = 48,
		R8G8_UNORM = 49,
		R8G8_UINT = 50,
		R8G8_SNORM = 51,
		R8G8_SINT = 52,
		R16_TYPELESS = 53,
		R16_FLOAT = 54,
		D16_UNORM = 55,
		R16_UNORM = 56,
		R16_UINT = 57,
		R16_SNORM = 58,
		R16_SINT = 59,
		R8_TYPELESS = 60,
		R8_UNORM = 61,
		R8_UINT = 62,
		R8_SNORM = 63,
		R8_SINT = 64,
		A8_UNORM = 65,
		R1_UNORM = 66,
		//R9G9B9E5_SHAREDEXP = 67,
		//R8G8_B8G8_UNORM = 68,
		//G8R8_G8B8_UNORM = 69,
		//BC1_TYPELESS = 70,
		//BC1_UNORM = 71,
		//BC1_UNORM_SRGB = 72,
		//BC2_TYPELESS = 73,
		//BC2_UNORM = 74,
		//BC2_UNORM_SRGB = 75,
		//BC3_TYPELESS = 76,
		//BC3_UNORM = 77,
		//BC3_UNORM_SRGB = 78,
		//BC4_TYPELESS = 79,
		//BC4_UNORM = 80,
		//BC4_SNORM = 81,
		//BC5_TYPELESS = 82,
		//BC5_UNORM = 83,
		//BC5_SNORM = 84,
		B5G6R5_UNORM = 85,
		B5G5R5A1_UNORM = 86,
		B8G8R8A8_UNORM = 87,
		B8G8R8X8_UNORM = 88,
		R10G10B10_XR_BIAS_A2_UNORM = 89,
		B8G8R8A8_TYPELESS = 90,
		B8G8R8A8_UNORM_SRGB = 91,
		B8G8R8X8_TYPELESS = 92,
		B8G8R8X8_UNORM_SRGB = 93,
		//BC6H_TYPELESS = 94,
		//BC6H_UF16 = 95,
		//BC6H_SF16 = 96,
		//BC7_TYPELESS = 97,
		//BC7_UNORM = 98,
		//BC7_UNORM_SRGB = 99,
		//AYUV = 100,
		//Y410 = 101,
		//Y416 = 102,
		//NV12 = 103,
		//P010 = 104,
		//P016 = 105,
		//_420_OPAQUE = 106,
		//YUY2 = 107,
		//Y210 = 108,
		//Y216 = 109,
		//NV11 = 110,
		//AI44 = 111,
		//IA44 = 112,
		//P8 = 113,
		//A8P8 = 114,
		//B4G4R4A4_UNORM = 115,
		//FORCE_UINT = 0xffffffff
	};
#endif

#ifdef OPENGL
	enum class pixel_format
	{
		UNKNOWN = 0,
		//TYPELESS
		R32G32B32A32_FLOAT = GL_RGBA32F,
		R32G32B32A32_UINT = GL_RGBA32UI,
		R32G32B32A32_SINT = GL_RGBA32I,
		//TYPELESS
		R32G32B32_FLOAT = GL_RGB32F,
		R32G32B32_UINT = GL_RGB32UI,
		R32G32B32_SINT = GL_RGB32I,
		//TYPELESS
		R16G16B16A16_FLOAT = GL_RGBA16F,
		R16G16B16A16_UINT = GL_RGBA16UI,
		R16G16B16A16_SINT = GL_RGBA16I,
		R16G16B16A16_SNORM = GL_RGBA16_SNORM,
		R16G16B16A16_UNORM = GL_RGBA16_SNORM,
		//TYPELESS
		R32G32_FLOAT = GL_RG32F,
		R32G32_UINT = GL_RG32UI,
		R32G32_SINT = GL_RG32I,
		//TYPELESS R32G8X24
		D32_FLOAT_S8X24_UINT = GL_DEPTH32F_STENCIL8,
		//TL R32_FLOAT_X8X24
		//TL X32_G8X24
		R8G8B8A8_UNORM = GL_RG8_SNORM,
		R8G8B8A8_SNORM = GL_RG8_SNORM,
		R8G8B8A8_UINT = GL_RG8UI,
		R8G8B8A8_SINT = GL_RG8I,
		D32_FLOAT = GL_DEPTH_COMPONENT32F,
		R32_UINT = GL_R32UI,
		R32_SINT = GL_R32I,
		R32_FLOAT = GL_R32F,

		//AND STUFF	 these seem useless

	};
#endif

	//ErrorCodeexception
	// exception the resulted from a error code that is failing
	struct error_code_exception : public exception
	{
	public:
		uint errorcode;
		error_code_exception(uint ec, const char* m = "") : errorcode(ec), exception(m) { }
	};

	//datablob<T>
	// pointer to a T along with the T's size, usually file data
	template<typename T>
	struct datablob
	{
		datablob() : data(nullptr), length(-1) { }
		datablob(T* d, size_t l) : data(d), length(l) { }
		datablob(size_t l) : data(new T[l]), length(l) { }

		~datablob()
		{
			delete[] data;
		}

		T* data;
		size_t length;
	};
	//Read in the data contained in filename, put it in to a datablob
	inline datablob<byte>* read_data(const wstring& filename);

	//Wrapper for read_data, but adds the executable path on to the file name
	inline datablob<byte>* read_data_from_package(_In_ const wstring& filename);
};
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


#ifdef OPENGL
#include <GL/glew.h>
#include <GL/wglew.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
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

#define array_size(ar) sizeof(ar) / sizeof(ar[0])

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

	//pixel_format
	//format of a pixel, used to describe textures, render targets
	enum class pixel_format
	{
		UNKNOWN = DXGI_FORMAT_UNKNOWN,
		RGBA32_TYPELESS = DXGI_FORMAT_R32G32B32A32_TYPELESS,
		RGBA32_FLOAT,// = DXGI_FORMAT_R32G32B32A32_FLOAT,
		RGBA32_UINT,// = DXGI_FORMAT_R32G32B32A32_UINT,
		RGBA32_SINT,// = DXGI_FORMAT_R32G32B32A32_SINT,
		RGB32_TYPELESS = DXGI_FORMAT_R32G32B32_TYPELESS,
		RGB32_FLOAT,// = DXGI_FORMAT_R32G32B32_FLOAT,
		RGB32_UINT,// = DXGI_FORMAT_R32G32B32_UINT,
		RGB32_SINT,// = DXGI_FORMAT_R32G32B32_SINT,
		RGBA16_TYPELESS = DXGI_FORMAT_R16G16B16A16_TYPELESS,
		RGBA16_FLOAT,// = DXGI_FORMAT_R16G16B16A16_FLOAT,
		RGBA16_UINT,// = DXGI_FORMAT_R16G16B16A16_UINT,
		RGBA16_SINT,// = DXGI_FORMAT_R16G16B16A16_SINT,
		RGBA16_UNORM,// = DXGI_FORMAT_R16G16B16A16_UNORM,
		RGBA16_SNORM,// = DXGI_FORMAT_R16G16B16A16_SNORM,
		RG32_TYPELESS = DXGI_FORMAT_R32G32_TYPELESS,
		RG32_FLOAT,// = DXGI_FORMAT_R32G32_FLOAT,
		RG32_UINT,// = DXGI_FORMAT_R32G32_UINT,
		RG32_SINT,// = DXGI_FORMAT_R32G32_SINT,
		RGBA8_TYPELESS = DXGI_FORMAT_R8G8B8A8_TYPELESS,
		RGBA8_UNORM,// = DXGI_FORMAT_R8G8B8A8_UNORM,
		RGBA8_UINT,// = DXGI_FORMAT_R8G8B8A8_TYPELESS,
		RGBA8_SNORM,// = DXGI_FORMAT_R8G8B8A8_TYPELESS,
		RGBA8_SINT,// = DXGI_FORMAT_R8G8B8A8_TYPELESS,
		RG16_TYPELESS = DXGI_FORMAT_R16G16_TYPELESS,
		RG16_FLOAT,
		RG16_UINT,
		RG16_UNORM,
		RG16_SINT,
		RG16_SNORM,
		R32_TYPELESS = DXGI_FORMAT_R32_TYPELESS,
		_D32_FLOAT,
		R32_FLOAT,
		R32_UINT,
		R32_SINT,
		R16_TYPELESS = DXGI_FORMAT_R16_TYPELESS,
		R16_FLOAT,
		R16_UNORM,
		R16_UINT,
		R16_SNORM,
		R16_SINT,
		R8_TYPELESS = DXGI_FORMAT_R8_TYPELESS,
		R8_UNORM,
		R8_UINT,
		R8_SNORM,
		R8_SINT,
	};	//this exploits the orderly nature of DXGI_FORMAT enum. if it changes, then this will need to reflect it.
#endif

#ifdef OPENGL	
	enum class pixel_format
	{
		UNKNOWN = GL_NONE,
		RGBA32_TYPELESS = GL_RGBA32UI,
		RGBA32_FLOAT = GL_RGBA32F,
		RGBA32_UINT = GL_RGBA32UI,
		RGBA32_SINT = GL_RGBA32I,
		RGB32_TYPELESS = GL_RGB32UI,
		RGB32_FLOAT = GL_RGB32F,
		RGB32_UINT = GL_RGB32UI,
		RGB32_SINT = GL_RGB32I,
		RGBA16_TYPELESS = GL_RGBA16UI,
		RGBA16_FLOAT = GL_RGBA16F,
		RGBA16_UINT = GL_RGBA16UI,
		RGBA16_SINT = GL_RGBA16I,
		RGBA16_UNORM = GL_RGBA16,
		RGBA16_SNORM = GL_RGBA16_SNORM,
		RG32_TYPELESS = GL_RG32UI,
		RG32_FLOAT = GL_RG32F,
		RG32_UINT = GL_RG32UI,
		RG32_SINT = GL_RG32I,
		RGBA8_TYPELESS = GL_RGBA8UI,
		RGBA8_UNORM = GL_RGBA8,
		RGBA8_UINT = GL_RGBA8UI,
		RGBA8_SNORM = GL_RGBA8_SNORM,
		RGBA8_SINT = GL_RGBA8I,
		RG16_TYPELESS = GL_RG16UI,
		RG16_FLOAT = GL_RG16F,
		RG16_UINT = GL_RG16UI,
		RG16_SINT = GL_RG16I,
		RG16_UNORM = GL_RG16,
		RG16_SNORM = GL_RG16_SNORM,
		R32_TYPELESS = GL_R32UI,
		R32_FLOAT = GL_R32F,
		R32_UINT = GL_R32UI,
		R32_SINT = GL_R32I,
		R16_TYPELESS = GL_R16UI,
		R16_FLOAT = GL_R16F,
		R16_UNORM = GL_R16,
		R16_UINT = GL_R32UI,
		R16_SNORM = GL_R16_SNORM,
		R16_SINT = GL_R32I,
		R8_TYPELESS = GL_R8UI,
		R8_UNORM = GL_R8,
		R8_UINT = GL_R8UI,
		R8_SNORM = GL_R8_SNORM,
		R8_SINT = GL_R8I,
	};

	//get_gl_format_type
	// returns values like GL_FLOAT
	GLenum get_gl_format_type(pixel_format f);

	//get_gl_format_internal
	// returns values like GL_RGBA
	GLenum get_gl_format_internal(pixel_format f);
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
	datablob<byte>* read_data(const wstring& filename);

	//Wrapper for read_data, but adds the executable path on to the file name
	datablob<byte>* read_data_from_package(_In_ const wstring& filename);
};
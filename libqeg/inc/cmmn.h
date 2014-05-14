#pragma once

//turn off stupid warnings
#define _CRT_SECURE_NO_WARNINGS

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <stack>
#include <queue>
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
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#ifdef WIN32
#include <Windows.h>
#ifdef LINK
#pragma comment(lib, "xinput.lib")
#endif
#endif

#ifdef DIRECTX
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <d3dcompiler.h>
#ifdef LINK
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#endif
#endif


#ifdef OPENGL
#include <GL/glew.h>
#include <GL/wglew.h>
#ifdef LINK
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "SOIL.lib")
#endif
#endif

#include "dostream.h"

//deal with stupid windows header and other fools
#undef min
#undef max

namespace qeg
{

	inline float randf() { return ((float)rand() / (float)RAND_MAX); }
	inline float randfn() { return ((randf() * 2) - 1); }

#define array_size(ar) sizeof(ar) / sizeof(ar[0])

//#define check_flag(f, v) (((uint)(v)&(uint)(f))==(uint)(f))

	template <typename flag_type>
	inline bool check_flag(flag_type check_for, flag_type in)
	{
		return (in&check_for) == check_for;
	}

#define pi glm::pi<float>()

	static auto cdlog =
#ifdef _DEBUG
#ifdef WIN32
		detail::dostream();
#elif
		std::cerr;
#endif
#elif
		detail::null_stream();
#endif

	enum class texture_filter
	{
		point, linear, anisotropic
	};

	enum class texture_dimension : uint
	{
		texture_1d = 0,
		texture_2d = 1,
		texture_3d = 2,
		texture_cube = 3,
	};

#ifdef OPENGL
#define opengl_exempt(x)
#define directx_exempt(x) x
#elif DIRECTX
#define opengl_exempt(x) x
#define directx_exempt(x) 
#endif

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
	//format of a pixel, used to describe textures, render targets, buffers
	enum class pixel_format : uint
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
		D32_FLOAT,
		R32_FLOAT,
		R32_UINT,
		R32_SINT,
		R16_TYPELESS = DXGI_FORMAT_R16_TYPELESS,
		R16_FLOAT,
		D16_UNORM,
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

	enum class texture_address_mode
	{
		wrap = 1, //lines up with D3D11_TEXTURE_ADDRESS_MODE
		mirror,
		clamp,
		border,
		mirror_once,
	};

	namespace detail
	{
		struct CD3D11_SUBRESOURCE_DATA : public D3D11_SUBRESOURCE_DATA
		{
			CD3D11_SUBRESOURCE_DATA(void* sys_mem, size_t sys_pitch, size_t sys_pitch_slice)
			{
				pSysMem = sys_mem;
				SysMemPitch = sys_pitch;
				SysMemSlicePitch = sys_pitch_slice;
			}
		};
	}
#endif

#ifdef OPENGL	
	//pixel_format
	//format of a pixel, used to describe textures, render targets, buffers
	enum class pixel_format : uint
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
		RGBA8_UNORM = GL_RGBA,
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
		R16_UINT = GL_R16UI,
		R16_SNORM = GL_R16_SNORM,
		R16_SINT = GL_R16I,
		R8_TYPELESS = GL_R8UI,
		R8_UNORM = GL_R8,
		R8_UINT = GL_R8UI,
		R8_SNORM = GL_R8_SNORM,
		R8_SINT = GL_R8I,
		BGR8_UNORM = GL_BGR,
	};

	enum class texture_address_mode
	{
		wrap = GL_REPEAT,
		mirror = GL_MIRRORED_REPEAT,
		clamp = GL_CLAMP_TO_EDGE,
		border = GL_CLAMP_TO_BORDER,
		mirror_once = GL_MIRROR_CLAMP_TO_EDGE,
	};

	namespace detail
	{
		//get_gl_txdm_from_pi
		// translates texture_dimension::texture_Xd/cube => GL_TEXTURE_XD or _CUBE_MAP
		inline GLenum get_gl_txdm_from_pi(texture_dimension td)
		{
			switch (td)
			{
			case texture_dimension::texture_1d:
				return GL_TEXTURE_1D;
			case texture_dimension::texture_2d:
				return GL_TEXTURE_2D;
			case texture_dimension::texture_3d:
				return GL_TEXTURE_3D;
			case texture_dimension::texture_cube:
				return GL_TEXTURE_CUBE_MAP;
			default:
				throw exception("unknown texture dimension");
			}
		}
	
		//get_gl_format_type
		// returns values like GL_FLOAT
		GLenum get_gl_format_type(pixel_format f);
	
		//get_gl_format_internal
		// returns values like GL_RGBA
		GLenum get_gl_format_internal(pixel_format f);

		uint get_size(pixel_format f);

		void __check_gl();
	}
#define check_gl 
	//detail::__check_gl();
#endif

	size_t bits_per_pixel(pixel_format fmt);
	inline size_t bytes_per_pixel(pixel_format fmt) { return bits_per_pixel(fmt) / 8; }

	//error_code_exception
	// exception the resulted from a error code that is failing
	struct error_code_exception : public exception
	{
	public:
		uint errorcode;
		error_code_exception(uint ec, const string& m = "") : errorcode(ec), exception(m.c_str()) { }
	};

	//datablob<T>
	// pointer to a T along with the T's size, usually file data
	template<typename T>
	struct datablob
	{
		datablob() : data(nullptr), length(-1) { }
		datablob(T* d, size_t l) : data(new T[l]), length(l) 
		{
			memcpy(data, d, l*sizeof(T));
		}
		datablob(size_t l) : data(new T[l]), length(l) { }

		~datablob()
		{
			if(data != nullptr)
				delete data;
			data = nullptr;
			length = -1;
		}

		inline bool empty() const { return data == nullptr; }

		T* data;
		size_t length;
	};


	//Read in the data contained in filename, put it in to a datablob
	const datablob<byte>& read_data(const wstring& filename);

	//Wrapper for read_data, but adds the executable path on to the file name
	const datablob<byte>& read_data_from_package(_In_ const wstring& filename);

	enum class shader_stage
	{
		pixel_shader, vertex_shader, //... other shaders
	};

	template <int Dim, class T, precision P>
	struct vec_of
	{
	public:
		typedef int x;
	};

	template <class T, precision P>
	struct vec_of<1, T, P>
	{
	public:
		typedef glm::detail::tvec1<T, P> x;
	};
	template <class T, precision P>
	struct vec_of<2, T, P>
	{
	public:
		typedef glm::detail::tvec2<T, P> x;
	};
	template <class T, precision P>
	struct vec_of<3, T, P>
	{
	public:
		typedef glm::detail::tvec3<T, P> x;
	};
	template <class T, precision P>
	struct vec_of<4, T, P>
	{
	public:
		typedef glm::detail::tvec4<T, P> x;
	};

};

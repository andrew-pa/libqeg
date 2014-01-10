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
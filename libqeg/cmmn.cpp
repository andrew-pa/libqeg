#include "cmmn.h"

namespace qeg
{
#ifdef WIN32
	//Read in the data contained in filename, put it in to a datablob
	datablob<byte>* read_data(
		const wstring& filename	//_In_ const wchar_t* filename
		)
	{
		CREATEFILE2_EXTENDED_PARAMETERS extendedParams = { 0 };
		extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
		extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
		extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
		extendedParams.lpSecurityAttributes = nullptr;
		extendedParams.hTemplateFile = nullptr;

		HANDLE file = CreateFile2(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams);

		if (file == INVALID_HANDLE_VALUE)
		{
			throw exception("DX::ReadData > Invalid File");
		}

		FILE_STANDARD_INFO fileInfo = { 0 };
		if (!GetFileInformationByHandleEx(
			file,
			FileStandardInfo,
			&fileInfo,
			sizeof(fileInfo)
			))
		{
			throw exception("DX::ReadData > GetFileInfo failed");
		}

		if (fileInfo.EndOfFile.HighPart != 0)
		{
			throw exception();
		}

		//datablob<byte> fileData = ref new Platform::Array<byte>(fileInfo.EndOfFile.LowPart);

		datablob<byte>* fileData = new datablob<byte>(fileInfo.EndOfFile.LowPart);

		DWORD al = 0;
		if (!ReadFile(
			file,
			fileData->data,
			fileData->length,
			&al,
			nullptr
			))
		{
			throw exception("DX::ReadData > Couldn't read file");
		}

		CloseHandle(file);
		return fileData;
	}

	//Wrapper for read_data, but adds the executable path on to the file name
	datablob<byte>* read_data_from_package(_In_ const wstring& filename)
	{
		static std::wstring fpath = L"";
		if (fpath.length() == 0)
		{
			wchar_t* modfn = new wchar_t[MAX_PATH];
			GetModuleFileName(NULL, modfn, MAX_PATH);
			fpath = std::wstring(modfn);
			int pl = -1;
			for (int i = fpath.length(); i > 0; i--)
			{
				if (fpath[i] == '\\')
				{
					pl = i + 1;
					break;
				}
			}
			fpath = fpath.substr(0, pl);
		}
		std::wstring fpn = fpath;
		fpn += filename;
		return read_data(fpn.data());
	}
#endif

#ifdef OPENGL
#define ogl_exempt(x)
#elif DIRECTX
#define ogl_exempt(x) x
#endif
	size_t bits_per_pixel(pixel_format fmt)
	{
		switch (fmt)
		{
		case pixel_format::RGBA32_TYPELESS:
		case pixel_format::RGBA32_FLOAT:
		ogl_exempt(case pixel_format::RGBA32_UINT:)
		case pixel_format::RGBA32_SINT:
			return 128;

		case pixel_format::RGB32_TYPELESS:
		case pixel_format::RGB32_FLOAT:
		ogl_exempt(case pixel_format::RGB32_UINT:)
		case pixel_format::RGB32_SINT:
			return 96;

		case pixel_format::RGBA16_TYPELESS:
		case pixel_format::RGBA16_FLOAT:
		case pixel_format::RGBA16_UNORM:
		ogl_exempt(case pixel_format::RGBA16_UINT:)
		case pixel_format::RGBA16_SNORM:
		case pixel_format::RGBA16_SINT:
		case pixel_format::RG32_TYPELESS:
		case pixel_format::RG32_FLOAT:
		ogl_exempt(case pixel_format::RG32_UINT:)
		case pixel_format::RG32_SINT:
			return 64;

		case pixel_format::RGBA8_TYPELESS:
		case pixel_format::RGBA8_UNORM:
		ogl_exempt(case pixel_format::RGBA8_UINT:)
		case pixel_format::RGBA8_SNORM:
		case pixel_format::RGBA8_SINT:
		case pixel_format::RG16_TYPELESS:
		case pixel_format::RG16_FLOAT:
		case pixel_format::RG16_UNORM:
		ogl_exempt(case pixel_format::RG16_UINT:)
		case pixel_format::RG16_SNORM:
		case pixel_format::RG16_SINT:
		case pixel_format::R32_TYPELESS:
		ogl_exempt(case pixel_format::_D32_FLOAT:)
		case pixel_format::R32_FLOAT:
		ogl_exempt(case pixel_format::R32_UINT:)
		case pixel_format::R32_SINT:
			return 32;

		case pixel_format::R16_TYPELESS:
		case pixel_format::R16_FLOAT:
		ogl_exempt(case pixel_format::_D16_UNORM:)
		case pixel_format::R16_UNORM:
		ogl_exempt(case pixel_format::R16_UINT:)
		case pixel_format::R16_SNORM:
		ogl_exempt(case pixel_format::R16_SINT:)
			return 16;

		case pixel_format::R8_TYPELESS:
		case pixel_format::R8_UNORM:
		ogl_exempt(case pixel_format::R8_UINT:)
		case pixel_format::R8_SNORM:
		case pixel_format::R8_SINT:
			return 8;

		default:
			return 0;
		}
	}

#ifdef OPENGL
	namespace detail
	{
	GLenum get_gl_format_type(pixel_format f)
	{
		switch (f)
		{
		case pixel_format::RGBA16_FLOAT:
			return GL_HALF_FLOAT;
		case pixel_format::RGBA32_FLOAT:
		case pixel_format::RGB32_FLOAT:
		case pixel_format::RG32_FLOAT:
		case pixel_format::RG16_FLOAT:
		case pixel_format::R32_FLOAT:
		case pixel_format::R16_FLOAT:
			return GL_FLOAT;

		case pixel_format::RGBA16_UINT:
			return GL_UNSIGNED_SHORT;
		case pixel_format::RGBA32_UINT:
		case pixel_format::RGB32_UINT:
		case pixel_format::RGBA8_UINT:
		case pixel_format::RG32_UINT:
		case pixel_format::RG16_UINT:
		case pixel_format::R32_UINT:
		//case pixel_format::R16_UINT:
		case pixel_format::R8_UINT:
			return GL_UNSIGNED_INT;

		case pixel_format::RGBA16_SINT:
			return GL_UNSIGNED_SHORT;
		case pixel_format::RGBA32_SINT:
		case pixel_format::RGB32_SINT:
		case pixel_format::RGBA8_SINT:
		case pixel_format::RG32_SINT:
		case pixel_format::RG16_SINT:
		case pixel_format::R32_SINT:
		//case pixel_format::R16_SINT:
		case pixel_format::R8_SINT:
			return GL_UNSIGNED_INT;

		case pixel_format::RGBA16_UNORM:
			return GL_UNSIGNED_SHORT;
		case pixel_format::RGBA8_UNORM:
			return GL_UNSIGNED_BYTE;
		case pixel_format::RG16_UNORM:
		case pixel_format::R16_UNORM:
		case pixel_format::R8_UNORM:
			return GL_UNSIGNED_INT;

		case pixel_format::RGBA16_SNORM:
			return GL_SHORT;
		case pixel_format::RGBA8_SNORM:
			return GL_BYTE;
		case pixel_format::RG16_SNORM:
		case pixel_format::R16_SNORM:
		case pixel_format::R8_SNORM:
			return GL_SHORT;
		default:
			throw exception("invalid format");
			break;
		}
	}
	GLenum get_gl_format_internal(pixel_format f)
	{
		switch (f)
		{
		//case pixel_format::RGBA32_TYPELESS:
		case pixel_format::RGBA32_UINT:
		case pixel_format::RGBA32_SINT:
		//case pixel_format::RGBA16_TYPELESS:
		case pixel_format::RGBA16_UINT:
		case pixel_format::RGBA16_SINT:
		//case pixel_format::RGBA8_TYPELESS:
		case pixel_format::RGBA8_UINT:
		case pixel_format::RGBA8_SINT:
			return GL_RGBA_INTEGER;
		case pixel_format::RGBA32_FLOAT:
		case pixel_format::RGBA16_FLOAT:
		case pixel_format::RGBA8_UNORM:
		case pixel_format::RGBA8_SNORM:
			return GL_RGBA;

		case pixel_format::RGB32_UINT:
		case pixel_format::RGB32_SINT:
		//case pixel_format::RGB32_TYPELESS:
			return GL_RGB_INTEGER;
		case pixel_format::RGB32_FLOAT:
			return GL_RGB;

		//case pixel_format::RG32_TYPELESS:
		case pixel_format::RG32_UINT:
		case pixel_format::RG32_SINT:
			return GL_RG_INTEGER;
		case pixel_format::RG32_FLOAT:
			return GL_RG;

		//case pixel_format::RG16_TYPELESS:
		case pixel_format::RG16_UINT:
		case pixel_format::RG16_SINT:
			return GL_RG_INTEGER;
		case pixel_format::RG16_FLOAT:
		case pixel_format::RG16_UNORM:
		case pixel_format::RG16_SNORM:
			return GL_RG;

		case pixel_format::R32_SINT:
		case pixel_format::R32_UINT:
		//case pixel_format::R32_TYPELESS:
			return GL_RED_INTEGER;
		case pixel_format::R32_FLOAT:
			return GL_RED;

		//case pixel_format::R16_SINT:
		//case pixel_format::R16_UINT:
		//case pixel_format::R16_TYPELESS:
		//	return GL_RED_INTEGER;
		case pixel_format::R16_FLOAT:
		case pixel_format::R16_UNORM:
		case pixel_format::R16_SNORM:
			return GL_RED;

		case pixel_format::R8_SINT:
		case pixel_format::R8_UINT:
		//case pixel_format::R8_TYPELESS:
			return GL_RED_INTEGER;
		case pixel_format::R8_SNORM:
		case pixel_format::R8_UNORM:
			return GL_RED;

		default:
			throw exception("invalid format");
		}
	}

	uint get_size(pixel_format f)
	{
		switch (f)
		{
		case pixel_format::RGBA16_FLOAT:
			return 16 * 4; //GL_HALF_FLOAT;
		case pixel_format::RGBA32_FLOAT:
		case pixel_format::RGB32_FLOAT:
		case pixel_format::RG32_FLOAT:
		case pixel_format::RG16_FLOAT:
		case pixel_format::R32_FLOAT:
		case pixel_format::R16_FLOAT:
			return GL_FLOAT;

		case pixel_format::RGBA16_UINT:
			return sizeof(unsigned short)*4;
		case pixel_format::RGBA32_UINT:
			return sizeof(unsigned int) * 3;
		case pixel_format::RGB32_UINT:
			return sizeof(unsigned int)*3;
		case pixel_format::RGBA8_UINT:
			return sizeof(unsigned char)*4;
		case pixel_format::RG32_UINT:
			return sizeof(unsigned int)*2;
		case pixel_format::RG16_UINT:
			return sizeof(unsigned short)*2;
		case pixel_format::R32_UINT:
			return sizeof(unsigned int);
			//case pixel_format::R16_UINT:
		case pixel_format::R8_UINT:
			return sizeof(unsigned char);

		case pixel_format::RGBA16_SINT:
			return sizeof(unsigned short)*4;
		case pixel_format::RGBA32_SINT:
			return sizeof(unsigned int)* 4;
		case pixel_format::RGB32_SINT:
			return sizeof(unsigned int)* 3;
		case pixel_format::RGBA8_SINT:
			return sizeof(unsigned char)* 4;
		case pixel_format::RG32_SINT:
			return sizeof(unsigned int)* 2;
		case pixel_format::RG16_SINT:
			return sizeof(unsigned short)*2;
		case pixel_format::R32_SINT:
			return sizeof(unsigned int);
			//case pixel_format::R16_SINT:
		case pixel_format::R8_SINT:
			return sizeof(unsigned short);

		case pixel_format::RGBA16_UNORM:
			return sizeof(unsigned short)* 4;
		case pixel_format::RGBA8_UNORM:
			return sizeof(unsigned char)* 4;
		case pixel_format::RG16_UNORM:
			return sizeof(unsigned short)* 2;
		case pixel_format::R16_UNORM:
			return sizeof(unsigned short);
		case pixel_format::R8_UNORM:
			return sizeof(unsigned char);

		case pixel_format::RGBA16_SNORM:
			return sizeof(unsigned short)* 4;
		case pixel_format::RGBA8_SNORM:
			return sizeof(unsigned char)* 4;
		case pixel_format::RG16_SNORM:
			return sizeof(unsigned short)* 2;
		case pixel_format::R16_SNORM:
			return sizeof(unsigned short);
		case pixel_format::R8_SNORM:
			return sizeof(unsigned char);
		default:
			throw exception("invalid format");
			break;
		}
	}

	void __check_gl()
	{
		GLenum e;
		if ((e = glGetError()) != GL_NO_ERROR)
		{
			cdlog << "GL error: " << e << endl;
			throw error_code_exception(e, "GL error");
		}
	}
	}
#endif

};
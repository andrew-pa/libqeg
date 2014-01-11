#include "cmmn.h"

namespace qeg
{
#ifdef WIN32
	//Read in the data contained in filename, put it in to a datablob
	inline datablob<byte>* read_data(
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

		if (!ReadFile(
			file,
			fileData->data,
			fileData->length,
			nullptr,
			nullptr
			))
		{
			throw exception("DX::ReadData > Couldn't read file");
		}

		CloseHandle(file);
		return fileData;
	}

	//Wrapper for read_data, but adds the executable path on to the file name
	inline datablob<byte>* read_data_from_package(_In_ const wstring& filename)
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

#ifdef DIRECTX
#undef RGB
#define FT_SWITCH_STATMENT(pps, type) switch (t)	\
	{	\
case qeg::pixel_format_type::RGB:	\
	return DXGI_FORMAT_R##pps##G##pps##B##pps##_##type;		\
case qeg::pixel_format_type::RGBA:	\
	return DXGI_FORMAT_R##pps##G##pps##B##pps##A##pps##_##type;	\
case qeg::pixel_format_type::BGR:	\
	return DXGI_FORMAT_B##pps##G##pps##R##pps##_##type;		\
case qeg::pixel_format_type::BGRA:	\
	return DXGI_FORMAT_B##pps##G##pps##R##pps##A##pps##_##type;	\
case qeg::pixel_format_type::DEPTH:	\
	return DXGI_FORMAT_D##pps##_##type; \
case qeg::pixel_format_type::R:	\
	return DXGI_FORMAT_R##pps##_##type; \
case qeg::pixel_format_type::RG:	\
	return DXGI_FORMAT_R##pps##G##pps##_##type; \
case qeg::pixel_format_type::UNKNOWN:	\
	break;	\
default:	\
	throw exception("invalid format");	\
	break;	\
	}\


	DXGI_FORMAT convert_format(pixel_format_type t, pixel_format_size_type s, uint pps)
	{
		if (t == pixel_format_type::UNKNOWN) return DXGI_FORMAT_UNKNOWN;
		switch (s)
		{
		case qeg::pixel_format_size_type::typeless:
			if(pps == 32) FT_SWITCH_STATMENT(32, TYPELESS)
			if(pps == 16) FT_SWITCH_STATMENT(16, TYPELESS)

			break;
		case qeg::pixel_format_size_type::float_:
			break;
		case qeg::pixel_format_size_type::uint:
			break;
		case qeg::pixel_format_size_type::sint:
			break;
		case qeg::pixel_format_size_type::unorm:
			break;
		case qeg::pixel_format_size_type::snorm:
			break;
		default:
			throw exception("invalid format");
			break;
		}
	}
#endif
#ifdef OPENGL
#endif
};
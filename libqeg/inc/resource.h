#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	namespace detail
	{
		/*
1	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RG8_UNORM"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	36	3	txcv
2	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RGB16_UNORM"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	37	3	txcv
4	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RG8_SNORM"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	40	3	txcv
5	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RGB16_SNORM"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	41	3	txcv
6	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "R32_SNORM"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	42	3	txcv
7	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RG8_UINT"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	44	3	txcv
8	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RGB16_UINT"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	45	3	txcv
9	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RG8_SINT"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	48	3	txcv
10	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RGB16_SINT"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	49	3	txcv
11	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "RGB16_FLOAT"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	52	3	txcv

3	IntelliSense: enum "qeg::detail::pi_pixel_format" has no member "R32_UNORM"	c:\Users\andre_000\Source\libqeg\txcv\src\main.cpp	38	3	txcv

	RG8, RGB16 ;, R32
*/
		enum class pi_pixel_format
		{
			UNKNOWN = 0,

			RGBA32_TYPELESS,
			RGBA32_FLOAT,
			RGBA32_UINT,
			RGBA32_SINT,

			RGB32_TYPELESS,
			RGB32_FLOAT,
			RGB32_UINT,
			RGB32_SINT,

			RGBA16_TYPELESS,
			RGBA16_FLOAT,
			RGBA16_UINT,
			RGBA16_SINT,
			RGBA16_UNORM,
			RGBA16_SNORM,

			RG32_TYPELESS,
			RG32_FLOAT,
			RG32_UINT,
			RG32_SINT,

			RGBA8_TYPELESS,
			RGBA8_UNORM,
			RGBA8_UINT,
			RGBA8_SNORM,
			RGBA8_SINT,

			RG16_TYPELESS,
			RG16_FLOAT,
			RG16_UINT,
			RG16_UNORM,
			RG16_SINT,
			RG16_SNORM,

			R32_TYPELESS,
			D32_FLOAT,
			R32_FLOAT,
			R32_UINT,
			R32_SINT,

			RG8_TYPELESS,
			RG8_UNORM,
			RG8_UINT,
			RG8_SNORM,
			RG8_SINT,

			R16_TYPELESS,
			R16_FLOAT,
			D16_UNORM,
			R16_UNORM,
			R16_UINT,
			R16_SNORM,
			R16_SINT,

			R8_TYPELESS,
			R8_UNORM,
			R8_UINT,
			R8_SNORM,
			R8_SINT,

			//crazy unused formats as -1! do not use!
			//TODO: implement these formats
			RGB8_UNORM = -1,
			RGB16_UNORM = -1,
			R32_UNORM = -1,
			RGB8_SNORM = -1,
			RGB16_SNORM = -1,
			RG32_UNORM = -1,
			R32_SNORM = -1,
			RGB8_UINT = -1,
			RGB32_UNORM = -1,
			RGB32_SNORM = -1,
			RG32_SNORM = -1,
			RGBA32_UNORM = -1,
			RGBA32_SNORM = -1,
			RGB16_UINT = -1,
			RGB8_SINT = -1,
			RGB16_SINT = -1,
			RGB16_FLOAT = -1,

		};

		pixel_format convert(pi_pixel_format f);

		pi_pixel_format convert(pixel_format f);

		struct texture_header
		{
			texture_dimension dim;
			uvec3 size;
			uint array_count; //or 0 for not a texture array
			pi_pixel_format format;
			uint mip_count; //or 0 for no mips (will use GPU to generate)
		};

		void* _load_texture(device* dev, const datablob<byte>& file_data);
	}
}
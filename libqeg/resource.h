#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	namespace detail
	{
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
			_D32_FLOAT,
			R32_FLOAT,
			R32_UINT,
			R32_SINT,
			R16_TYPELESS,
			R16_FLOAT,
			_D16_UNORM,
			R16_UNORM,
			R16_UINT,
			R16_SNORM,
			R16_SINT,
			R8_TYPELESS,
			R8_UNORM,
			R8_UINT,
			R8_SNORM,
			R8_SINT,
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

		void* _load_texture(device* dev, datablob<byte>* file_data);
	}
}
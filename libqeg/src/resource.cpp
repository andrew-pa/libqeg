#include "resource.h"
#include "bo_file.h"
#include "texture.h"

namespace qeg
{
	namespace detail
	{
#define ___conv_ipp(x) case pi_pixel_format:: x: return pixel_format:: x;
#define ___conv_pip(x) case pixel_format:: x: return pi_pixel_format:: x;


		pixel_format convert(pi_pixel_format f)
		{
			switch (f)
			{
				___conv_ipp(UNKNOWN)
					___conv_ipp(RGBA32_TYPELESS)
					___conv_ipp(RGBA32_FLOAT)
					___conv_ipp(RGBA32_UINT)
					___conv_ipp(RGBA32_SINT)
					___conv_ipp(RGB32_TYPELESS)
					___conv_ipp(RGB32_FLOAT)
					___conv_ipp(RGB32_UINT)
					___conv_ipp(RGB32_SINT)
					___conv_ipp(RGBA16_TYPELESS)
					___conv_ipp(RGBA16_FLOAT)
					___conv_ipp(RGBA16_SINT)
					___conv_ipp(RGBA16_UNORM)
					___conv_ipp(RGBA16_SNORM)
					___conv_ipp(RG32_TYPELESS)
					___conv_ipp(RG32_FLOAT)
					___conv_ipp(RG32_UINT)
					___conv_ipp(RG32_SINT)
					___conv_ipp(RGBA8_TYPELESS)
					___conv_ipp(RGBA8_UNORM)
					___conv_ipp(RGBA8_UINT)
					___conv_ipp(RGBA8_SNORM)
					___conv_ipp(R32_TYPELESS)
					___conv_ipp(R32_FLOAT)
					___conv_ipp(R32_UINT)
					___conv_ipp(R32_SINT)
					___conv_ipp(R16_TYPELESS)
					___conv_ipp(R16_FLOAT)
					___conv_ipp(R16_SINT)
					___conv_ipp(R8_TYPELESS)
					___conv_ipp(R8_UNORM)
					___conv_ipp(R8_UINT)
					___conv_ipp(R8_SNORM)
					___conv_ipp(R8_SINT)

			default:
				throw exception("invalid pi_pixel_format");
			}
		}

		pi_pixel_format convert(pixel_format f)
		{
			switch (f)
			{
				___conv_pip(UNKNOWN)
					opengl_exempt(___conv_pip(RGBA32_TYPELESS))
					___conv_pip(RGBA32_FLOAT)
					___conv_pip(RGBA32_UINT)
					___conv_pip(RGBA32_SINT)
					opengl_exempt(___conv_pip(RGB32_TYPELESS))
					___conv_pip(RGB32_FLOAT)
					___conv_pip(RGB32_UINT)
					___conv_pip(RGB32_SINT)
					opengl_exempt(___conv_pip(RGBA16_TYPELESS))
					___conv_pip(RGBA16_FLOAT)
					___conv_pip(RGBA16_SINT)
					___conv_pip(RGBA16_UNORM)
					___conv_pip(RGBA16_SNORM)
					opengl_exempt(___conv_pip(RG32_TYPELESS))
					___conv_pip(RG32_FLOAT)
					___conv_pip(RG32_UINT)
					___conv_pip(RG32_SINT)
					opengl_exempt(___conv_pip(RGBA8_TYPELESS))
					___conv_pip(RGBA8_UNORM)
					___conv_pip(RGBA8_UINT)
					___conv_pip(RGBA8_SNORM)
					opengl_exempt(___conv_pip(R32_TYPELESS))
					___conv_pip(R32_FLOAT)
					___conv_pip(R32_UINT)
					___conv_pip(R32_SINT)
					opengl_exempt(___conv_pip(R16_TYPELESS))
					___conv_pip(R16_FLOAT)
					___conv_pip(R16_UINT)
					___conv_pip(R16_UNORM)
					___conv_pip(R16_SINT)
					___conv_pip(R16_SNORM)
					opengl_exempt(___conv_pip(R8_TYPELESS))
					___conv_pip(R8_UNORM)
					___conv_pip(R8_UINT)
					___conv_pip(R8_SNORM)
					___conv_pip(R8_SINT)

			default:
				throw exception("invalid pixel_format");
			}
		}

		void* _load_texture(device* dev, const datablob<byte>& file_data)
		{
			bo_file f(file_data);
			if (f.type() != bo_file::file_type::texture)
				throw exception("file not a texture");

			texture_header* h = nullptr;
			vector<uint> data_chunk_indices;
			for (uint i = 0; i < f.chunks().size(); ++i)
			{
				auto c = f.chunks()[i];
				if (c.type == 0)
				{
					//found header chunk
					if (c.data->length < sizeof(texture_header))
						throw exception("texture header chunk too small");
					h = (texture_header*)c.data->data;
				}
				else if (c.type == 1)
				{
					//found data chunk
					data_chunk_indices.push_back(i);
				}
			}

			pixel_format h_format = convert(h->format);

			bool gen_mips = h->mip_count <= 1;

			if (data_chunk_indices.size() == 0) throw exception("no data chunks");

			if (h == nullptr) throw exception("no texture header found");

			if (h->dim == texture_dimension::texture_1d)
			{
				if (h->array_count > 0)
				{
					throw exception("texture arrays not supported yet");
				}
				else
				{
					auto c = f.chunks()[data_chunk_indices[0]];
					auto bpp = bytes_per_pixel(h_format);
					if (!gen_mips)
					{
						uint off = 0;
						vector<size_t> offsets;
						for (uint i = 0; i < h->mip_count; ++i)
						{
							uint msize = (uint)floor((h->size.x) / pow(2.f, (float)i));
							offsets.push_back(off);
							off += msize;
						}
						off *= bpp;
						if (off > c.data->length) throw exception("not enougth data for all mipmaps in chunk");
						vector<void*> data;
						for (const auto& ol : offsets)
						{
							data.push_back(c.data->data + (ol*bpp));
						}
						return new texture1d(dev, h->size.x, h_format,
							data);
					}
					if (c.data->length < (h->size[0] * bytes_per_pixel(h_format)))
						throw exception("not enough data in chunk");
					return new texture1d(dev, h->size[0], h_format, c.data->data);
				}
			}
			else if (h->dim == texture_dimension::texture_2d)
			{
				if (h->array_count > 0)
				{
					throw exception("texture arrays not supported yet");
				}
				else
				{
					auto c = f.chunks()[data_chunk_indices[0]];
					auto bpp = bytes_per_pixel(h_format);
					uint len = (h->size[0] * h->size[1] * bpp);
					if (!gen_mips) //image contains mips
					{
						uint off = 0;
						vector<size_t> offsets;
						for (uint i = 0; i < h->mip_count; ++i)
						{
							uvec2 msize = (uvec2)floor(vec2(h->size) / pow(2.f, (float)i));
							offsets.push_back(off);
							off += msize.x * msize.y;
						}
						off *= bpp;
						if (off > c.data->length) throw exception("not enougth data for all mipmaps in chunk");
						vector<void*> data;
						for (const auto& ol : offsets)
						{
							data.push_back(c.data->data + (ol*bpp));
						}
						return new texture2d(dev, uvec2(h->size[0], h->size[1]), h_format,
							data);
					}
					if (c.data->length < len)
						throw exception("not enough data in chunk");
					return new texture2d(dev, uvec2(h->size[0], h->size[1]), h_format,
						c.data->data);
				}
			}
			else if (h->dim == texture_dimension::texture_3d)
			{
				if (h->array_count > 0)
				{
					throw exception("texture arrays not supported yet");
				}
				else
				{
					auto c = f.chunks()[data_chunk_indices[0]];
					if (c.data->length < (h->size[0] * h->size[1] * h->size[2] * bytes_per_pixel(h_format)))
						throw exception("not enough data in chunk");
					return new texture3d(dev, h->size, h_format,
						c.data->data, gen_mips, h->size[2] * bytes_per_pixel(h_format),
						bytes_per_pixel(h_format)*h->size.y);
				}
			}
			else if (h->dim == texture_dimension::texture_cube)
			{
				if (h->array_count > 6)
				{
					throw exception("texture arrays not supported yet");
				}
				else
				{
					if (f.chunks().size() < 7) throw exception("not enough chunks for entire cubemap");
					vector<byte*> facedata;
					for (int i = 0; i < 6; ++i)
					{
						auto c = f.chunks()[data_chunk_indices[i]];
						if (c.data->length < (h->size[0] * h->size[1] * bytes_per_pixel(h_format)))
							throw exception("not enough data in chunk");
						facedata.push_back(c.data->data);
					}
					return new textureCube(dev, uvec2(h->size[0], h->size[1]), h_format,
						facedata, true, bytes_per_pixel(h_format)*h->size.y);
				}
			}

			return nullptr;
		}
	}
}

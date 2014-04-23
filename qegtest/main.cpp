#define LINK
#include <cmmn.h>
#include <device.h>
#include <app.h>
#include <shader.h>
#include <mesh.h>
#include <constant_buffer.h>
#include <camera.h>
#include <aux_cameras.h>
#include <basic_input.h>
#include <texture.h>
#include <states.h>
using namespace qeg;

#include <glm/gtc/noise.hpp>

mesh* create_box(device* _dev, const string& name, float d, bool interleaved = false)
{
	typedef vertex_position_normal_texture dvertex;
	vertex_position_normal_texture* v = new vertex_position_normal_texture[24];

	float w2 = 0.5f*d;
	float h2 = 0.5f*d;
	float d2 = 0.5f*d;

	// Fill in the front face vertex data. 
	v[0] = dvertex(-w2, -h2, -d2,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f, 
		0.0f, 1.0f);
	v[1] = dvertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = dvertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = dvertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = dvertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = dvertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = dvertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = dvertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = dvertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = dvertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = dvertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = dvertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = dvertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = dvertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = dvertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = dvertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = dvertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = dvertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = dvertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = dvertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = dvertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = dvertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = dvertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = dvertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	uint16* i = new uint16[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	if(interleaved)
	{
		return new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, 
			vector<vertex_position_normal_texture>(v, &v[24]), vector<uint16>(i, &i[36]), name);
	}

	auto indexCount = 36;
	auto vertexCount = 24;
	vector<vec3> p, n;
	vector<vec2> t;
	for (int i = 0; i < 24; ++i)
	{
		p.push_back(v[i].pos);
		n.push_back(v[i].norm);
		t.push_back(v[i].tex);
	}
	return new mesh_psnmtx(_dev, p, n, t, vector<uint16>(i, i + 36), name);
}

mesh* create_sphere(device* device, float radius, uint sliceCount, uint stackCount, const string& mesh_name, bool interleaved = false)
{
	typedef vertex_position_normal_texture dvertex;

	vector<vec3> p, n;
	vector<vec2> t;
	std::vector<uint16> indices;

	dvertex topv(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	dvertex botv(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	p.push_back(vec3(0, radius, 0)); n.push_back(vec3(0, 1, 0)); t.push_back(vec2(0, 0));

	float dphi = pi / stackCount;
	float dtheta = 2.0f*pi / sliceCount;

	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*dphi;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*dtheta;
			vec3 pos;
			pos.x = radius * sinf(phi) * cosf(theta);
			pos.y = radius * cosf(phi);
			pos.z = radius * sinf(phi) * sinf(theta);
			p.push_back(pos);
			n.push_back(normalize(pos));
			t.push_back(vec2(theta / pi*2, phi / pi));
			/*auto P = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.norm, XMVector3Normalize(P));
			v.texc = float2(theta / XM_2PI, phi / XM_PI);*/
			//vertices->push_back(v);
		}
	}

	p.push_back(vec3(0, -radius, 0)); n.push_back(vec3(0, -1, 0)); t.push_back(vec2(0, 1));

	//vertices->push_back(botv);

	for (uint16 i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	uint16 bi = 1;
	uint16 rvc = sliceCount + 1;
	for (uint16 i = 0; i < stackCount - 2; ++i)
	{
		for (uint j = 0; j < sliceCount; ++j)
		{
			indices.push_back(bi + i*rvc + j);
			indices.push_back(bi + i*rvc + j + 1);
			indices.push_back(bi + (i + 1)*rvc + j);
			indices.push_back(bi + (i + 1)*rvc + j);
			indices.push_back(bi + (i*rvc + j + 1));
			indices.push_back(bi + (i + 1)*rvc + j + 1);
		}
	}

	uint16 spi = (uint16)p.size() - 1;
	bi = spi - rvc;
	for (uint i = 0; i < sliceCount; ++i)
	{
		indices.push_back(spi);
		indices.push_back(bi + i);
		indices.push_back(bi + i + 1);
	}

	if(interleaved)
	{
			vector<vertex_position_normal_texture> v;
			for (uint i = 0; i < p.size(); ++i)
			{
				v.push_back(vertex_position_normal_texture(p[i], n[i], t[i]));
			}
			return new interleaved_mesh<vertex_position_normal_texture, uint16>(device, v, indices, mesh_name);
	}

	return new mesh_psnmtx(device, p, n, t, indices, mesh_name);
}

class bo_file
{
public:
	enum class file_type : uint
	{
		generic = 0,
		texture = 8,
	};
	struct chunk
	{
		uint type;
		datablob<byte>* data;
		chunk(uint t, datablob<byte>* d)
			: type(t), data(d){}
	};
protected:
	struct header
	{
		file_type type;
		uint number_of_chunks;
	};
	struct chunk_desc
	{
		uint type;
		size_t offset;
		size_t length;
	};
	file_type _htype;
	vector<chunk> _chunks;
public:

	bo_file(file_type t)
		: _htype(t){}

	bo_file(datablob<byte>* d)
	{
		byte* bd = d->data;
		header* h = (header*)bd;
		bd += 2 * sizeof(uint);
		chunk_desc* cds = (chunk_desc*)bd;
		bd += sizeof(chunk_desc)*h->number_of_chunks;
		_htype = h->type;
		for (int i = 0; i < h->number_of_chunks; ++i)
		{
			_chunks.push_back(chunk(cds->type, new datablob<byte>(bd + cds->offset, cds->length)));
			cds++; //move to next chunk
		}
	}

	datablob<byte>* write()
	{
		size_t total_length = sizeof(header)+sizeof(chunk_desc)*_chunks.size();
		for (const auto& c : _chunks)
			total_length += c.data->length;

		byte* bd = new byte[total_length];
		byte* fbd = bd;
		header* h = (header*)bd;
		bd += sizeof(header);
		h->type = _htype;
		h->number_of_chunks = _chunks.size();

		byte* data_bd = bd + sizeof(chunk_desc)*_chunks.size();
		size_t current_offset = 0;

		for(const auto& c : _chunks)
		{
			chunk_desc* cd = (chunk_desc*)bd;
			cd->type = c.type;
			cd->offset = current_offset;
			cd->length = c.data->length;

			memcpy(data_bd, c.data->data, c.data->length);

			current_offset += c.data->length;
			data_bd += c.data->length;
			bd += sizeof(chunk_desc);
		}

		return new datablob<byte>(fbd, total_length);
	}

	proprw(file_type, type, { return _htype; });
	proprw(vector<chunk>, chunks, { return _chunks; });
};

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

	struct texture_header
	{
		texture_dimension dim;
		uvec3 size;
		uint array_count; //or 0 for not a texture array
		pi_pixel_format format;
		uint mip_count; //or 0 for no mips (will use GPU to generate)
	};

void* _load_texture(device* dev, datablob<byte>* file_data)
{
	bo_file f(file_data);
	if (f.type() != bo_file::file_type::texture)
		throw exception("file not a texture");

	texture_header* h = nullptr;
	vector<uint> data_chunk_indices;
	for (int i = 0; i < f.chunks().size(); ++i)
	{
		auto c = f.chunks()[i];
		if(c.type == 0)
		{
			//found header chunk
			if (c.data->length < sizeof(texture_header))
				throw exception("texture header chunk too small");
			h = (texture_header*)c.data->data;
		}
		else if(c.type == 1) 
		{
			//found data chunk
			data_chunk_indices.push_back(i);
		}
	}

	pixel_format h_format = convert(h->format);

	bool gen_mips =  h->mip_count == 0;
	
	if (data_chunk_indices.size() == 0) throw exception("no data chunks");

	if (h == nullptr) throw exception("no texture header found");

	if(h->dim == texture_dimension::texture_1d)
	{
		if(h->array_count > 0)
		{
			throw exception("texture arrays not supported yet");
		}
		else
		{
			auto c = f.chunks()[data_chunk_indices[0]];
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
			if(!gen_mips) //image contains mips
			{
				//offset, length
				//vector<pair<size_t, size_t>> mip_ofl;
				//mip_ofl.push_back(make_pair(  0, len)); //mip lvl 0
				//size_t off = len;
				//for (int i = 0; i < h->mip_count-1; i++)
				//{
				//	float m = pow(2.f, i);
				//	mip_ofl.push_back(make_pair(off, (size_t)floor((h->size.x / m) * (h->size.y / m)) * bpp));
				//	off += (size_t)floor((h->size.x / m) * (h->size.y / m)) * bpp;
				//}
				uint off = 0;
				vector<size_t> offsets;
				for (int i = 0; i < h->mip_count; ++i)
				{
					uvec2 msize = (uvec2)floor(vec2(h->size) / pow(2.f, (float)i));
					offsets.push_back(off);
					off += msize.x * msize.y;
				}
				off *= sizeof(vec4);
				if (off > c.data->length) throw exception("not enougth data for all mipmaps in chunk");
				vector<void*> data;
				for(const auto& ol : offsets)
				{
					data.push_back(c.data->data+(ol*bpp));
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
				c.data->data, gen_mips, h->size[2]*bytes_per_pixel(h_format), 
				bytes_per_pixel(h_format)*h->size.y);
		}
	}
	else if (h->dim == texture_dimension::texture_cube)
	{
		if (h->array_count > 0)
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
				facedata, gen_mips, bytes_per_pixel(h_format)*h->size.y);
		}
	}
	
	return nullptr;
}
}

inline texture1d* load_texture1d(device* dev, datablob<byte>* file_data) { return static_cast<texture1d*>(::detail::_load_texture(dev, file_data)); }
inline texture2d* load_texture2d(device* dev, datablob<byte>* file_data) { return static_cast<texture2d*>(::detail::_load_texture(dev, file_data)); }
inline texture3d* load_texture3d(device* dev, datablob<byte>* file_data) { return static_cast<texture3d*>(::detail::_load_texture(dev, file_data)); }
inline textureCube* load_textureCube(device* dev, datablob<byte>* file_data) { return static_cast<textureCube*>(::detail::_load_texture(dev, file_data)); }

class qegtest_app : public app
{
	mesh* m;
	shader s;
	struct wvpcbd
	{
		mat4 wvp;
		mat4 inw;
		vec4 t;
		wvpcbd() {}
		wvpcbd(mat4 a, mat4 b, vec4 _t)
			: wvp(a), inw(b), t(_t){}
	};
	constant_buffer<wvpcbd> wvp_cb;
	fps_camera c;

	texture2d* tx;
	sampler_state smpl;
public:
	qegtest_app()
		: app(
#ifdef DIRECTX
			L"libqeg test (DirectX)", 
#elif OPENGL
			L"libqeg test (OpenGL)",
#endif
			vec2(640, 480), false, 1.f/60.f),
		s(_dev, read_data_from_package(L"simple.vs.csh"), read_data_from_package(L"simple.ps.csh")),
		wvp_cb(_dev, s, 0, wvpcbd(), shader_stage::vertex_shader),
		c(vec3(0.5f, 8.f, -28), vec3(0,0,0.1), radians(40.f), _dev->size(), 10.f, 1.5f) //(45, 45, 5.f, vec3(0), radians(45.f), _dev->size())//(vec3(3, 2, -10), vec3(0, 0, 1), 45.f, _dev->size()),
		, smpl(_dev)
	{

		m = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev,
			generate_torus<vertex_position_normal_texture, uint16>(vec2(2, .7f), 64), "torus0");
			//generate_plane<vertex_position_normal_texture, uint16>(vec2(16), vec2(64), vec3(0, -1, 0)), "plane0");
			//generate_sphere<vertex_position_normal_texture,uint16>(2.f, 64, 64), "sphere0"); 
			//create_sphere(_dev, 1.3f, 64, 64, "s0", true); 
			//create_box(_dev, "box0", 1, true);

		//tx = texture2d::load_dds(_dev, read_data_from_package(L"test.dds"));

		//const int size = 512;

		//bo_file f(bo_file::file_type::texture);
		//::detail::texture_header h;
		//h.dim = texture_dimension::texture_2d;
		//h.size = uvec3(size, size, 0);
		//h.array_count = 0;
		//h.mip_count = 10;
		//h.format = ::detail::pi_pixel_format::RGBA32_FLOAT;
		////vec4* img = new vec4[size * size];
		////for (int y = 0; y < size; ++y)
		////	for (int x = 0; x < size; x++)
		////	{
		////		vec2 p = vec2((float)x / (float)size, (float)y / (float)size);
		////		float n = abs(perlin(p*32.f));
		////		img[x + y * size] = vec4(vec3(n), 1.f);
		////	}
		//uint fsize = 0;
		//vector<size_t> offsets;
		//for (int i = 0; i < h.mip_count; ++i)
		//{
		//	uvec2 msize = (uvec2)floor(vec2(size) / pow(2.f, (float)i));
		//	offsets.push_back(fsize);
		//	fsize += msize.x * msize.y;		
		//}
		// 
		//vec4* img = new vec4[fsize];
		//vector<vec3> pal(20);
		//generate(pal.begin(), pal.end(), [&] { return vec3(linearRand(vec3(.2f), vec3(.8f))); });
		//for (int i = 0; i < h.mip_count; ++i)
		//{
		//	vec4* im = img + offsets[i];
		//	uvec2 msize = (uvec2)floor(vec2(size) / pow(2.f, (float)i));
		//	for (int y = 0; y < msize.y; ++y)
		//		for (int x = 0; x < msize.x; ++x)
		//		{
		//			vec2 p = vec2((float)x / (float)msize.x, (float)y / (float)msize.y);
		//			float n = abs(perlin(p*32.f));
		//			im[x + (y * msize.y)] = vec4(n, n*.5f, 0,1.f);
		//		}
		//}
		//
		//bo_file::chunk hc(0, new datablob<byte>((byte*)&h, sizeof(::detail::texture_header)));
		//bo_file::chunk dc(1, new datablob<byte>((byte*)img, fsize*sizeof(vec4)));
		//f.chunks().push_back(hc);
		//f.chunks().push_back(dc);
		//auto txf = f.write();
		//
		//ofstream out("perlin.tex", ios_base::binary);
		//out.write((const char*)txf->data, txf->length);
		//out.flush();
		//out.close();

		tx = load_texture2d(_dev,
			read_data(L"C:\\Users\\andre_000\\Source\\libqeg\\qegtest\\perlin.tex"));

		//auto m = generate_plane<vertex_position_normal_texture, uint16>(vec2(10, 10), uvec2(16, 16), vec3(1, 0, 0));

		//generate_cube<vertex_position,uint16>(vec3(0, 0, 0));
	}

	void update(float t, float dt) override
	{
		c.update(dt);
		c.update_view();
		//auto world = mat4(1);// rotate(mat4(1), t * 100, vec3(.2f, .7f, .6f));
		//wvp_cb.data(wvpcbd(c.projection()*c.view()*world, glm::inverse(transpose(world)), vec4(t)));
	}
	
	void resized() override
	{
		c.update_proj(_dev->size());
	}

	void render(float t, float dt) override
	{
		s.bind(_dev);
		tx->bind(_dev, 0, shader_stage::pixel_shader, s);
		smpl.bind(_dev, 0, shader_stage::pixel_shader);
		wvp_cb.bind(_dev);

		auto world = mat4(1);
		wvp_cb.data(wvpcbd(c.projection()*c.view()*world, glm::inverse(transpose(world)), vec4(t, 0, 0, 0)));
		wvp_cb.update(_dev);
		m->draw(_dev);

		//for (float i = 0; i < 6; ++i)
		//{
		//world = translate(mat4(1), vec3(10*cosf(i+t*.4f), sinf((t+i*.3f)*3), 10*sinf(i+t*.4f)));
		//wvp_cb.data(wvpcbd(c.projection()*c.view()*world, glm::inverse(transpose(world)), vec4(t, i, 0, 0)));
		//wvp_cb.update(_dev);
		//m->draw(_dev);
		//}
		smpl.unbind(_dev, 0, shader_stage::pixel_shader);
		tx->unbind(_dev, 0, shader_stage::pixel_shader);
		wvp_cb.unbind(_dev);
		s.unbind(_dev);
	}
};

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR cmdLine, int cmdShow)
{
	qegtest_app a;
	a.run();
}















//
////random code
//
////ray gen -> intersect -> shade -> final
//
//struct ray { vec3 o, d; vec2 screen_pos; uint lvl; };
//
//vector<ray> gen_ray()
//{
//}
//
//struct hitrec { float t; vec3 n; vec2 texc; ray r; };
//
//vector<hitrec> intersect(const vector<ray>& r)
//{
//}
//
//vector<vec3> shade(const vector<hitrec>& hrs, vector<ray>& nxt)
//{
//}
//
//template <typename T>
//vector<T> op(const vector<T>& a, const vector<T>& b, function<T(const T& a, const T& b)> fn)
//{
//	vector<T> res;
//
//	for (int i = 0; i < a.size() && i < b.size(); ++i)
//	{
//		res.push_back(fn(a[i], b[i]));
//	}
//
//	return res;
//}
//
//vector<vec3> raytrace()
//{
//	vector<vec3> frmbuffer;
//	auto rs = gen_ray();
//	auto hr = intersect(rs);
//	vector<ray> nrs; vector<vec3> surf1;
//	auto surf0 = shade(hr, nrs);
//	uint idx = 0;
//	while(idx < 10)
//	{
//		auto r = op<vec3>(surf0, (surf1 = shade(intersect(nrs), rs)), [&](const vec3& a, const vec3& b) { return a * b; });
//		frmbuffer = op<vec3>(frmbuffer, r, [&](const vec3& a, const vec3& b) { return a + b; });
//		swap(rs, nrs);
//		rs.clear();
//		swap(surf0, surf1);
//		surf1.clear();
//		idx++;
//	}
//	return frmbuffer;
//}
//
////0 surf indrct0
////surf*indrct0 surf1 indrct1
////surf*indrct0 + surf1*indrct1
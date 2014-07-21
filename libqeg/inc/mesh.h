#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	template <typename index_type>
	struct format_for_index_type
	{
		static const pixel_format format = pixel_format::UNKNOWN;
	};

	template <>
	struct format_for_index_type<uint32>
	{
		static const pixel_format format = pixel_format::R32_UINT;
	};

	template <>
	struct format_for_index_type<uint16>
	{
		static const pixel_format format = pixel_format::R16_UINT;
	};

#ifdef OPENGL
	//gl_vertex_attrib
	//	represents a vertex attribute, is GL equivalent to D3D11_INPUT_ELEMENT_DESC struct
	struct gl_vertex_attrib
	{
		GLuint idx;
		GLuint count;
		GLuint offset;
		GLenum type;
		gl_vertex_attrib(GLuint i, GLuint c, GLenum t, GLuint o)
			: idx(i), count(c), type(t), offset(o) {}
		inline void apply(GLuint stride)
		{
			glVertexAttribPointer(idx, count, type, GL_FALSE, stride, (void*)offset);
		}
	};
#endif

#define DEF_VERTEX_MC(t) t(vec3 position, vec3 normal, vec3 tangent, vec2 texcoords)

	//vertex_position
	//	vertex with only position coord
	struct vertex_position
	{
#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC d3d_input_layout[];
#endif
#ifdef OPENGL
		static vector<gl_vertex_attrib> get_vertex_attribs();
#endif
		vec3 pos;
		vertex_position(vec3 p)
			: pos(p)
		{
		}

		DEF_VERTEX_MC(vertex_position)
			: pos(position) {}
	};

	//vertex_position_normal_texture
	// vertex with position coords, normal, and texture coords. Frequently used
	struct vertex_position_normal_texture
	{
#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC d3d_input_layout[];
#endif
#ifdef OPENGL
		static vector<gl_vertex_attrib> get_vertex_attribs();
#endif
		vec3 pos;
		vec3 norm;
		vec2 tex;
		vertex_position_normal_texture(){}
		vertex_position_normal_texture(vec3 p, vec3 n, vec2 t)
			: pos(p), norm(n), tex(t)
		{
		}
		vertex_position_normal_texture(float px, float py, float pz, float nx, float ny, float nz, float tx = 0, float ty = 0, float tz = 0, float u = 0, float v = 0)
			: pos(px, py, pz), norm(nx, ny, nz), tex(tx, ty){}

		DEF_VERTEX_MC(vertex_position_normal_texture)
			: pos(position), norm(normal), tex(texcoords) {}
	};

	//vertex_position_normal_tangent_texture
	// vertex with position coords, normal, tangent, and texture coords. Frequently used with bump mapping
	struct vertex_position_normal_tangent_texture
	{
#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC d3d_input_layout[];
#endif
#ifdef OPENGL
		static vector<gl_vertex_attrib> get_vertex_attribs();
#endif
		vec3 pos;
		vec3 norm;
		vec3 tang;
		vec2 tex;
		vertex_position_normal_tangent_texture(){}
		vertex_position_normal_tangent_texture(vec3 p, vec3 n, vec3 tg, vec2 t)
			: pos(p), norm(n), tang(tg), tex(t)
		{
		}
	};

	//prim_draw_type
	//	Describes how the GPU should handle the vertex data
	enum class prim_draw_type
	{
#ifdef DIRECTX
		triangle_list = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		triangle_strip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		line_list = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		line_strip = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		point_list = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
#endif
#ifdef OPENGL
		triangle_list = GL_TRIANGLES,
		triangle_strip = GL_TRIANGLE_STRIP,
		list_list = GL_LINES,
		line_strip = GL_LINE_STRIP,
		point_list = GL_POINTS,
#endif
	};

	template <typename vertex_type, typename index_type>
	struct sys_mesh
	{
		vector<vertex_type> vertices;
		vector<index_type> indices;
	};

	//mesh
	//	base class for all meshes
	class mesh
	{
	protected:
#ifdef OPENGL
		GLuint vtx_array;
#endif
		string _name;
	public:
		mesh(const string& n);
		proprw(string, name, { return _name; });
		virtual void draw(device* _dev, prim_draw_type dt = prim_draw_type::triangle_list, 
			int index_offset = 0, int oindex_count = -1, int vertex_offset = 0) = 0;
		~mesh();
#ifdef OPENGL
		propr(GLuint, vertex_array, { return vtx_array; });
#endif
	};
	
	//interleaved_mesh<vertex_type, index_type>
	//	interleaved_mesh is a mesh that uses interleaved vertices, where each vertex is a struct
	//	vertex_type: vertex data, as a struct. see vertex_position and vertex_position_normal_texture for examples of implementation. 
	//	index_type:  type for index values. uint16 or uint32 usually
	template<typename vertex_type, typename index_type> class interleaved_mesh : public mesh
	{
		uint idx_cnt;
		uint vtx_cnt;
#ifdef DIRECTX
		ComPtr<ID3D11Buffer> vtx_buf;
		ComPtr<ID3D11Buffer> idx_buf;
#elif OPENGL
			GLuint vtx_buf;
			GLuint idx_buf;
#endif
	public:
		interleaved_mesh(device* _dev, const vector<vertex_type>& vs, const vector<index_type>& is, 
			const string& name)
			: mesh(name), vtx_cnt(vs.size()), idx_cnt(is.size())
#ifdef DIRECTX
		{
			CD3D11_BUFFER_DESC vd(sizeof(vertex_type)*vs.size(), D3D11_BIND_VERTEX_BUFFER);
			CD3D11_BUFFER_DESC id(sizeof(index_type)*is.size(), D3D11_BIND_INDEX_BUFFER);
			D3D11_SUBRESOURCE_DATA sdd = {0};
			sdd.pSysMem = vs.data();
			chr(_dev->ddevice()->CreateBuffer(&vd, &sdd, &vtx_buf));
			sdd.pSysMem = is.data();
			chr(_dev->ddevice()->CreateBuffer(&id, &sdd, &idx_buf));
		}
#elif OPENGL
		{
			glBindVertexArray(vtx_array);
			glGenBuffers(1, &vtx_buf);
			glGenBuffers(1, &idx_buf);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buf);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, is.size()*sizeof(index_type), is.data(), GL_STATIC_DRAW);
			
			glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);
			glBufferData(GL_ARRAY_BUFFER, vs.size()*sizeof(vertex_type), vs.data(), GL_STATIC_DRAW);

			auto vabs = vertex_type::get_vertex_attribs();
			for (const auto& v : vabs)
			{
				glEnableVertexAttribArray(v.idx);
				glVertexAttribPointer(v.idx, v.count, v.type, GL_FALSE, sizeof(vertex_type), (void*)v.offset);
			}
		}
#endif
		interleaved_mesh(device* _dev, const sys_mesh<vertex_type,index_type>& gm, const string& nm)
			: interleaved_mesh(_dev, gm.vertices, gm.indices, nm)
		{}
		
		void draw(device* _dev, prim_draw_type dt = prim_draw_type::triangle_list,
			int index_offset = 0, int oindex_count = -1, int vertex_offset = 0) override
#ifdef DIRECTX
		{
			uint strides[] = { sizeof(vertex_type) };
			uint offsets[] = {0};
			_dev->context()->IASetVertexBuffers(0, 1, vtx_buf.GetAddressOf(), strides, offsets);
			_dev->context()->IASetIndexBuffer(idx_buf.Get(), (DXGI_FORMAT)format_for_index_type<index_type>().format, 0);
			_dev->context()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)dt);
			_dev->context()->DrawIndexed((oindex_count > 0 ? oindex_count : idx_cnt), index_offset, vertex_offset);

		}
#elif OPENGL
		{
			glBindVertexArray(vtx_array);			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buf);
			glDrawElements((GLenum)dt, (oindex_count == -1 ? idx_cnt : oindex_count),
				GL_UNSIGNED_SHORT, (void*)0); //TODO: Change GL_UNSIGNED_SHORT to whatever the appropriate GL_* const is for the index_type
		}
#endif
		
		~interleaved_mesh()
#ifdef DIRECTX
		{
		}
#elif OPENGL
		{
			glDeleteBuffers(1, &vtx_buf);
			glDeleteBuffers(1, &idx_buf);
		}
#endif

		propr(uint, index_count, { return idx_cnt; });
		propr(uint, vertex_count, { return vtx_cnt; });
#ifdef DIRECTX
		propr(ID3D11Buffer*, vertex_buffer, { return vtx_buf; });
		propr(ID3D11Buffer*, index_buffer, { return idx_buf; });
#elif OPENGL
		propr(GLuint, vertex_buffer, { return vtx_buf; });
		propr(GLuint, index_buffer, { return idx_buf; });
#endif
	};

	template <typename vertex_type, typename index_type>
	sys_mesh<vertex_type,index_type> generate_sphere(float radius, uint slice_count, uint stack_count)
	{
		sys_mesh<vertex_type, index_type> m;

		m.vertices.push_back(vertex_type(vec3(0.f, radius, 0.f), vec3(0, 1, 0), vec3(1, 0, 0), vec2(0, 0)));

		float dphi = pi / stack_count;
		float dtheta = 2.f*pi / slice_count;

		for (uint i = 1; i <= stack_count - 1; ++i)
		{
			float phi = i*dphi;
			for (uint j = 0; j <= slice_count; ++j)
			{
				float theta = j*dtheta;
				vec3 p = vec3(radius*sinf(phi)*cosf(theta),
							  radius*cosf(phi),
							  radius*sinf(phi)*sinf(theta));
				vec3 t = normalize(vec3(-radius*sinf(phi)*sinf(theta),
					0.f,
					radius*sinf(phi)*cosf(theta)));
				m.vertices.push_back(vertex_type(p, normalize(p), t, vec2(theta / (2.f*pi), phi / (2.f*pi))));

			}
		}

		m.vertices.push_back(vertex_type(vec3(0.f, -radius, 0.f), vec3(0, -1, 0), vec3(1, 0, 0), vec2(0, 1)));

		for (index_type i = 1; i <= slice_count; ++i)
		{
			m.indices.push_back(0);
			m.indices.push_back(i + 1);
			m.indices.push_back(i);
		}

		index_type bi = 1;
		index_type rvc = slice_count + 1;
		for (index_type i = 0; i < stack_count - 2; ++i)
		{
			for (uint j = 0; j < slice_count; ++j)
			{
				m.indices.push_back(bi + i*rvc + j);
				m.indices.push_back(bi + i*rvc + j + 1);
				m.indices.push_back(bi + (i + 1)*rvc + j);
				m.indices.push_back(bi + (i + 1)*rvc + j);
				m.indices.push_back(bi + (i*rvc + j + 1));
				m.indices.push_back(bi + (i + 1)*rvc + j + 1);
			}
		}

		index_type spi = (index_type)m.vertices.size() - 1;
		bi = spi - rvc;
		for (uint i = 0; i < slice_count; ++i)
		{
			m.indices.push_back(spi);
			m.indices.push_back(bi + i);
			m.indices.push_back(bi + i + 1);
		}

		return m;
	}
	
	template <typename vertex_type, typename index_type>
	sys_mesh<vertex_type, index_type> generate_plane(vec2 dims, vec2 div, vec3 norm = vec3(0,1,0))
	{
		sys_mesh<vertex_type, index_type> m;

		vec3 nw = normalize(norm);
		vec3 t = (fabsf(nw.x) > .1 ? vec3(0, 1, 0) : vec3(1, 0, 0));
		vec3 nu = normalize(cross(t, nw));
		vec3 nv = cross(nw, nu);

		vec2 hdims = .5f*dims;

		vec2 dxy = dims / (div - vec2(1));

		vec2 duv = 1.f / (div - vec2(1));

		for (float i = 0; i < div.y; ++i)
		{
			float y = hdims.y - i*dxy.y;
			for (float j = 0; j < div.x; ++j)
			{
				float x = hdims.x - j*dxy.x;
				
				vec3 p = nu*x + nv*y;

				m.vertices.push_back(vertex_type(p, -nw, nu, vec2(j,i)*duv));
			}
		}

		for (uint i = 0; i < div.x - 1; ++i)
		{
			for (uint j = 0; j < div.y - 1; ++j)
			{
				m.indices.push_back(i*(uint16)div.y + j);
				m.indices.push_back(i*(uint16)div.y + j + 1);
				m.indices.push_back((i + 1)*(uint16)div.y + j);

				m.indices.push_back((i + 1)*(uint16)div.y + j);
				m.indices.push_back(i*(uint16)div.y + j + 1);
				m.indices.push_back((i + 1)*(uint16)div.y + j + 1);
			}
		}

		reverse(m.indices.begin(), m.indices.end());

		return m;
	}

	template <typename vertex_type, typename index_type>
	sys_mesh<vertex_type, index_type> generate_torus(vec2 r, int div)
	{
		int ring_count = div;
		int stack_count = div;
		sys_mesh<vertex_type, index_type> m;

		vector<vertex_position_normal_tangent_texture> frvtx;
		for (int i = 0; i < div + 1; ++i)
		{
			vec4 p = vec4(r.y, 0.f, 0.f,1.f)*rotate(mat4(1), radians(i*360.f / (float)div), vec3(0, 0, 1))
				+ vec4(r.x, 0.f, 0.f, 1.f);
			vec2 tx = vec2(0, (float)i / div);
			vec4 tg = vec4(0.f, -1.f, 0.f, 1.f)*rotate(mat4(1), radians(i*360.f / (float)div), vec3(0, 0, 1));
			vec3 n = cross(vec3(tg), vec3(0.f, 0.f, -1.f));
			m.vertices.push_back(vertex_type(vec3(p), vec3(n), vec3(tg), tx));
			frvtx.push_back(vertex_position_normal_tangent_texture(vec3(p), n, vec3(tg), tx));
		}

		for (int ring = 1; ring < ring_count + 1; ++ring)
		{
			mat4 rot = rotate(mat4(1), radians(ring*360.f / (float)div), vec3(0, 1, 0));
			for (int i = 0; i < stack_count + 1; ++i)
			{
				vec4 p = vec4(frvtx[i].pos.x, frvtx[i].pos.y, frvtx[i].pos.z, 1.f);
				vec4 nr = vec4(frvtx[i].norm.x, frvtx[i].norm.y, frvtx[i].norm.z, 0.f);
				vec4 tg = vec4(frvtx[i].tang.x, frvtx[i].tang.y, frvtx[i].tang.z, 0.f);
				p = p*rot;
				nr = nr*rot;
				tg = tg*rot;

				m.vertices.push_back(vertex_type(vec3(p), vec3(nr),
					vec3(tg), vec2(2.f*ring / (float)div, frvtx[i].tex.y)));
			}
		}

		for (int ring = 0; ring < ring_count; ++ring)
		{
			for (int i = 0; i < stack_count; ++i)
			{
				m.indices.push_back(ring*(div + 1) + i);
				m.indices.push_back((ring+1)*(div + 1) + i);
				m.indices.push_back(ring*(div + 1) + i +1);

				m.indices.push_back(ring*(div + 1) + i + 1);
				m.indices.push_back((ring+1)*(div + 1) + i);
				m.indices.push_back((ring+1)*(div + 1) + i + 1);
			}
		}
		
		return m;
	}

	//mutable_interleaved_mesh<vertex_type, index_type>
	//	interleaved_mesh is a mesh that uses interleaved vertices, where each vertex is a struct
	//	vertex_type: vertex data, as a struct. see vertex_position and vertex_position_normal_texture for examples of implementation. 
	//	index_type:  type for index values. uint16 or uint32 usually
	//	mutable_interleaved_mesh allows for updating the mesh from the CPU
	template<typename vertex_type, typename index_type> class mutable_interleaved_mesh : public mesh
	{
		uint idx_cnt;
		uint vtx_cnt;
#ifdef DIRECTX
		ComPtr<ID3D11Buffer> vtx_buf;
		ComPtr<ID3D11Buffer> idx_buf;
#elif OPENGL
		GLuint vtx_buf;
		GLuint idx_buf;
#endif
	public:
		mutable_interleaved_mesh(device* _dev, const vector<vertex_type>& vs, const vector<index_type>& is,
			const string& name)
			: mesh(name), vtx_cnt(vs.size()), idx_cnt(is.size())
#ifdef DIRECTX
		{
			CD3D11_BUFFER_DESC vd(sizeof(vertex_type)*vs.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			CD3D11_BUFFER_DESC id(sizeof(index_type)*is.size(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			D3D11_SUBRESOURCE_DATA sdd = { 0 };
			sdd.pSysMem = vs.data();
			chr(_dev->ddevice()->CreateBuffer(&vd, &sdd, &vtx_buf));
			sdd.pSysMem = is.data();
			chr(_dev->ddevice()->CreateBuffer(&id, &sdd, &idx_buf));
		}
#elif OPENGL
		{
			glBindVertexArray(vtx_array);
			glGenBuffers(1, &vtx_buf);
			glGenBuffers(1, &idx_buf);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buf);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, is.size()*sizeof(index_type), is.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);
			glBufferData(GL_ARRAY_BUFFER, vs.size()*sizeof(vertex_type), vs.data(), GL_DYNAMIC_DRAW);

			auto vabs = vertex_type::get_vertex_attribs();
			for (const auto& v : vabs)
			{
				glEnableVertexAttribArray(v.idx);
				glVertexAttribPointer(v.idx, v.count, v.type, GL_FALSE, sizeof(vertex_type), (void*)v.offset);
			}
		}
#endif
		mutable_interleaved_mesh(device* _dev, const sys_mesh<vertex_type, index_type>& gm, const string& nm)
			: mutable_interleaved_mesh(_dev, gm.vertices, gm.indices, nm)
		{}

		void update(device* _dev, const vector<vertex_type>& vs, const vector<index_type>& is)
#ifdef DIRECTX
		{

		}
#elif OPENGL
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buf);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, is.size()*sizeof(index_type), is.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);
			glBufferData(GL_ARRAY_BUFFER, vs.size()*sizeof(vertex_type), vs.data(), GL_DYNAMIC_DRAW);
		}
#endif

		void update(device* _dev, const sys_mesh<vertex_type, index_type>& gm)
		{
			update(_dev, gm.vertices, gm.indices);
		}
		
		void draw(device* _dev, prim_draw_type dt = prim_draw_type::triangle_list,
			int index_offset = 0, int oindex_count = -1, int vertex_offset = 0) override
#ifdef DIRECTX
		{
			uint strides[] = { sizeof(vertex_type) };
			uint offsets[] = { 0 };
			_dev->context()->IASetVertexBuffers(0, 1, vtx_buf.GetAddressOf(), strides, offsets);
			_dev->context()->IASetIndexBuffer(idx_buf.Get(), (DXGI_FORMAT)format_for_index_type<index_type>().format, 0);
			_dev->context()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)dt);
			_dev->context()->DrawIndexed((oindex_count > 0 ? oindex_count : idx_cnt), index_offset, vertex_offset);

		}
#elif OPENGL
		{
			glBindVertexArray(vtx_array);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buf);
			glDrawElements((GLenum)dt, (oindex_count == -1 ? idx_cnt : oindex_count),
				GL_UNSIGNED_SHORT, (void*)0); //TODO: Change GL_UNSIGNED_SHORT to whatever the appropriate GL_* const is for the index_type
		}
#endif

		~mutable_interleaved_mesh()
#ifdef DIRECTX
		{
		}
#elif OPENGL
		{
			glDeleteBuffers(1, &vtx_buf);
			glDeleteBuffers(1, &idx_buf);
		}
#endif

		propr(uint, index_count, { return idx_cnt; });
		propr(uint, vertex_count, { return vtx_cnt; });
#ifdef DIRECTX
		propr(ID3D11Buffer*, vertex_buffer, { return vtx_buf; });
		propr(ID3D11Buffer*, index_buffer, { return idx_buf; });
#elif OPENGL
		propr(GLuint, vertex_buffer, { return vtx_buf; });
		propr(GLuint, index_buffer, { return idx_buf; });
#endif
	};
	

	//use interleaved mesh instead!
	//mesh_psnmtx
	//	a mesh implementation that stores the positions, normals and texture coords of each vertex in a seperate stream.
	//	shader::layout_posnormtex layout works for this mesh
	class mesh_psnmtx : public mesh
	{
		uint idxcnt;
		//dont know if needed
		//vector<vec3> ps, nm;
		//vector<vec2> tx;
		//vector<uint16> ixs;
#ifdef DIRECTX
		ID3D11Buffer* bufs[4];
#endif
#ifdef OPENGL
		GLuint bufid[4];
#endif
	public:
		mesh_psnmtx(device* _dev, const vector<vec3>& pos, const vector<vec3>& norm, const vector<vec2>& tex,
			const vector<uint16>& indices, const string& n);
		void draw(device* _dev, prim_draw_type dt = prim_draw_type::triangle_list, 
			int index_offset = 0, int oindex_count = -1, int vertex_offset = 0) override;
		
		~mesh_psnmtx();
	
#ifdef OPENGL
		propr(GLuint, positions_buffer, { return bufid[0]; });
		propr(GLuint, normals_buffer, { return bufid[1]; });
		propr(GLuint, texcoords_buffer, { return bufid[2]; });
		propr(GLuint, index_buffer, { return bufid[3]; });
#endif
	};
}



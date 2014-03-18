#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	template <typename index_type>
	struct format_for_index_type
	{
		static const buffer_format format = buffer_format::UNKNOWN;
	};

	template <>
	struct format_for_index_type<uint32>
	{
		static const buffer_format format = buffer_format::R32_UINT;
	};

	template <>
	struct format_for_index_type<uint16>
	{
		static const buffer_format format = buffer_format::R16_UINT;
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

	//vertex_position
	//	vertex with only position coord
	struct vertex_position
	{
#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC d3d_input_layout[];
#endif
#ifdef OPENGL
		static gl_vertex_attrib* get_vertex_attribs();
#endif
		vec3 pos;
		vertex_position(vec3 p)
			: pos(p)
		{
		}
	};

	//vertex_position_normal_texture
	// vertex with position coords, normal, and texture coords. Frequently used
	struct vertex_position_normal_texture
	{
#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC d3d_input_layout[];
#endif
#ifdef OPENGL
		static gl_vertex_attrib* get_vertex_attribs();
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
		union
		{
			GLuint bufs[2];
			GLuint vtx_buf;
			GLuint idx_buf;
		};
#endif
	public:
		interleaved_mesh(device* _dev, const vector<vertex_type>& vs, const vector<index_type>& is, 
			const string& name)
			: mesh(name)
#ifdef DIRECTX
			, vtx_cnt(vs.size()), idx_cnt(is.size())
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
			glGenBuffers(2, bufs);
			
			glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);
			glBindBuffer(GL_ARRAY_BUFFER, vs.size()*sizeof(vertex_type), vs.data(), GL_STATIC_DRAW);
			auto vabs = vertex_type::get_vertex_attribs();
			for (const auto& v : vabs)
			{
				glEnableVertexAttribArray(v.idx);
				v.apply(sizeof(vertex_type));
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buf);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, is.size()*sizeof(index_type), is.data(), GL_STATIC_DRAW);
		}
#endif
		
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
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer());
			glDrawElements((GLenum)dt, (oindex_count == -1 ? idxcnt : oindex_count),
				GL_UNSIGNED_SHORT, (void*)0);
		}
#endif
		
		~interleaved_mesh()
#ifdef DIRECTX
		{
		}
#elif OPENGL
		{
			glDeleteBuffers(2, bufs);
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



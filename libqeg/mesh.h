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
	struct gl_vertex_attrib
	{
		GLuint idx;
		GLuint count;
		GLenum type;
		gl_vertex_attrib(GLuint i, GLuint c, GLenum t)
			: idx(i), count(c), type(t) {}
		inline void apply()
		{
			glVertexAttribPointer(idx, count, type, GL_FALSE, 0, (void*)0);
		}
	};
#endif

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
	
	/*template <typename vertex_type, typename index_type = uint32>
	class mesh
	{
		string _name;
		vector<vertex_type> _vertices;
		vector<index_type> _indices;
#ifdef DIRECTX
		ComPtr<ID3D11Buffer> vertex_buf;
		ComPtr<ID3D11Buffer> index_buf;
		ComPtr<ID3D11InputLayout> inplayo;
#endif
#ifdef OPENGL
		GLuint vtx_array;
		GLuint bufid[2];
#endif
	public:
		mesh(){}
		mesh(device* _dev, const string& n, const vector<vertex_type>& v, const vector<index_type>& i)
			: _name(n), _vertices(v), _indices(i)
#ifdef DIRECTX
		{
			D3D11_SUBRESOURCE_DATA vd = { 0 };
			vd.pSysMem = &v[0];
			CD3D11_BUFFER_DESC vbd(sizeof(vertex_type)*v.size(), D3D11_BIND_VERTEX_BUFFER);
			chr(_dev->ddevice()->CreateBuffer(&vbd, &vd, &vertex_buf));
			D3D11_SUBRESOURCE_DATA id = { 0 };
			id.pSysMem = &i[0];
			CD3D11_BUFFER_DESC ibd(sizeof(index_type)*i.size(), D3D11_BIND_INDEX_BUFFER);
			chr(_dev->ddevice()->CreateBuffer(&ibd, &id, &index_buf));
		}
#elif OPENGL
		{		
			glGenVertexArrays(1, &vtx_array);
			glBindVertexArray(vtx_array);
			glGenBuffers(2, bufid);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer());
			glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(vertex_type), &v[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer());
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size()*sizeof(index_type), &i[0], GL_STATIC_DRAW);
		}
#endif
		~mesh()
#ifdef DIRECTX
		{}
#elif OPENGL
		{		
			glDeleteVertexArrays(1, &vtx_array);
			glDeleteBuffers(2, bufid);
		}
#endif

		void draw(device* _dev, prim_draw_type drawtype = prim_draw_type::triangle_list)
#ifdef DIRECTX
		{
			UINT stride = sizeof(vertex_type);
			UINT offset = 0;
			_dev->context()->IASetVertexBuffers(0, 1, vertex_buf.GetAddressOf(), &stride, &offset);
			_dev->context()->IASetIndexBuffer(index_buf.Get(), (DXGI_FORMAT)format_for_index_type<index_type>::format, 0);
			_dev->context()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)drawtype);
			_dev->context()->DrawIndexed(_indices.size(), 0, 0);

		}
#elif  OPENGL	
		{
			glBindVertexArray(vtx_array);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer());
			gl_vertex_attrib* vtx_attrib = vertex_type::get_vertex_attribs();
			for (int i = 0; i < ARRAYSIZE(vtx_attrib); ++i)
			{
				glEnableVertexAttribArray(i);
				vtx_attrib[i].apply();
			}

			glDrawElements(drawtype, indices.size(), format_for_index_type<index_type>::format, (void*)0);
		}
#endif

		proprw(string, name, { return _name; });
		propr(vector<vertex_type>, vertices, { return _vertices; });
		propr(vector<index_type>, indices, { return _indices; });

#ifdef DIRECTX
		propr(ComPtr<ID3D11Buffer>, vertex_buffer, { return vertex_buf; });
		propr(ComPtr<ID3D11Buffer>, index_buffer, { return index_buf; });
		propr(ComPtr<ID3D11InputLayout>, input_layout, { return inplayo; });
#endif
#ifdef OPENGL
		propr(GLuint, vertex_array, { return vtx_array; });
		propr(GLuint, vertex_buffer, { return bufid[0]; });
		propr(GLuint, index_buffer, { return bufid[1]; });
#endif
	};*/

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



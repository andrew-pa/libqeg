#include "mesh.h"

namespace qeg
{


#ifdef DIRECTX
	const D3D11_INPUT_ELEMENT_DESC vertex_position::d3d_input_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	const D3D11_INPUT_ELEMENT_DESC vertex_position_normal_texture::d3d_input_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mesh::mesh(const string& n)
		: _name(n)
	{
	}
	mesh::~mesh()
	{
	}

	mesh_psnmtx::mesh_psnmtx(device* _dev, const vector<vec3>& pos, const vector<vec3>& norm, const vector<vec2>& tex,
		const vector<uint16>& indices, const string& n)
		: mesh(n), idxcnt(indices.size())//, ps(pos), nm(norm), tx(tex), ixs(indices)
	{
		CD3D11_BUFFER_DESC pos_desc(sizeof(vec3)*pos.size(), D3D11_BIND_VERTEX_BUFFER);
		CD3D11_BUFFER_DESC norm_desc(sizeof(vec3)*norm.size(), D3D11_BIND_VERTEX_BUFFER);
		CD3D11_BUFFER_DESC tex_desc(sizeof(vec2)*tex.size(), D3D11_BIND_VERTEX_BUFFER);
		CD3D11_BUFFER_DESC idx_desc(sizeof(uint16)*indices.size(), D3D11_BIND_INDEX_BUFFER);
		D3D11_SUBRESOURCE_DATA subdata = { 0 };
		subdata.pSysMem = pos.data();
		chr(_dev->ddevice()->CreateBuffer(&pos_desc, &subdata, &bufs[0]));
		subdata.pSysMem = norm.data();
		chr(_dev->ddevice()->CreateBuffer(&norm_desc, &subdata, &bufs[1]));
		subdata.pSysMem = tex.data();
		chr(_dev->ddevice()->CreateBuffer(&tex_desc, &subdata, &bufs[2]));
		subdata.pSysMem = indices.data();
		chr(_dev->ddevice()->CreateBuffer(&idx_desc, &subdata, &bufs[3]));
	}

	void mesh_psnmtx::draw(device* _dev, prim_draw_type dt,
		int index_offset, int oindex_count, int vertex_offset)
	{
		uint strides[3] = { sizeof(vec3), sizeof(vec3), sizeof(vec2), };
		uint offsets[3] = { 0, 0, 0, };
		_dev->context()->IASetVertexBuffers(0, 3, bufs, strides, offsets);
		_dev->context()->IASetIndexBuffer(bufs[3], DXGI_FORMAT_R16_UINT, 0);
		_dev->context()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)dt);
		_dev->context()->DrawIndexed((oindex_count > 0 ? oindex_count : idxcnt), index_offset, vertex_offset);
	}

	mesh_psnmtx::~mesh_psnmtx()
	{
		for (int i = 0; i < 4; ++i)
		{
			bufs[i]->Release();
			bufs[i] = nullptr;
		}
	}
	
	//template <typename vertex_type, typename index_type>
	//mesh<vertex_type,index_type>::mesh(device* _dev, const string& n, const vector<vertex_type>& v, 
	//	const vector<index_type>& i)
	//	: _name(n), _vertices(v), _indices(i)
	//{
	//	D3D11_SUBRESOURCE_DATA vd = { 0 };
	//	vd.pSysMem = &v[0];
	//	CD3D11_BUFFER_DESC vbd(sizeof(vertex_type)*v.size(), D3D11_BIND_VERTEX_BUFFER);
	//	chr(_dev->ddevice()->CreateBuffer(&vbd, &vd, &vertex_buf));
	//	D3D11_SUBRESOURCE_DATA id = { 0 };
	//	id.pSysMem = &i[0];
	//	CD3D11_BUFFER_DESC ibd(sizeof(index_type)*i.size(), D3D11_BIND_INDEX_BUFFER);
	//	chr(_dev->ddevice()->CreateBuffer(&ibd, &id, &index_buf));
	//}

	//template <typename vertex_type, typename index_type>
	//void mesh<vertex_type, index_type>::draw(device* _dev, prim_draw_type drawtype)
	//{
	//	UINT stride = sizeof(vertex_type);
	//	UINT offset = 0;
	//	_dev->context()->IASetVertexBuffers(0, 1, vertex_buf.GetAddressOf(), &stride, &offset);
	//	_dev->context()->IASetIndexBuffer(index_buf.Get(), (DXGI_FORMAT)format_for_index_type<index_type>::format, 0);
	//	_dev->context()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)drawtype);
	//	_dev->context()->DrawIndexed(_indices.size(), 0, 0);
	//
	//}

	//template <typename vertex_type, typename index_type>
	//mesh<vertex_type, index_type>::~mesh()
	//{
	//}

#endif

#ifdef OPENGL

	vector<gl_vertex_attrib> vertex_position::get_vertex_attribs()
	{
		static vector<gl_vertex_attrib> va = 
		{
			{ 0, 3, GL_FLOAT, offsetof(vertex_position, pos) },
		};
		return va;
	}

	vector<gl_vertex_attrib> vertex_position_normal_texture::get_vertex_attribs()
	{
		static vector<gl_vertex_attrib> va =
		{
			{ 0, 3, GL_FLOAT, offsetof(vertex_position_normal_texture, pos) },
			{ 1, 3, GL_FLOAT, offsetof(vertex_position_normal_texture, norm) },
			{ 2, 2, GL_FLOAT, offsetof(vertex_position_normal_texture, tex) },
		};
		return va;
	}


	mesh::mesh(const string& n)
		: _name(n)
	{
		glGenVertexArrays(1, &vtx_array); check_gl
	}
	mesh::~mesh()
	{
		//glDeleteVertexArrays(1, &vtx_array); check_gl
	}

	mesh_psnmtx::mesh_psnmtx(device* _dev, const vector<vec3>& pos, const vector<vec3>& norm, const vector<vec2>& tex,
		const vector<uint16>& indices, const string& n)
		: mesh(n), idxcnt(indices.size())//, ps(pos), nm(norm), tx(tex), ixs(indices)
	{
		glBindVertexArray(vtx_array); check_gl
			glGenBuffers(4, bufid); check_gl
		
			glBindBuffer(GL_ARRAY_BUFFER, positions_buffer()); check_gl
			glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(vec3), pos.data(), GL_STATIC_DRAW); check_gl
			glBindBuffer(GL_ARRAY_BUFFER, normals_buffer()); check_gl
			glBufferData(GL_ARRAY_BUFFER, norm.size()*sizeof(vec3), norm.data(), GL_STATIC_DRAW); check_gl
			glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer()); check_gl
			glBufferData(GL_ARRAY_BUFFER, tex.size()*sizeof(vec2), tex.data(), GL_STATIC_DRAW); check_gl
		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer()); check_gl
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16), indices.data(), GL_STATIC_DRAW); check_gl
	}

	void mesh_psnmtx::draw(device* _dev, prim_draw_type dt,
		int index_offset, int oindex_count, int vertex_offset)
	{
		glBindVertexArray(vtx_array); check_gl
		
		glEnableVertexAttribArray(0); check_gl
		glBindBuffer(GL_ARRAY_BUFFER, positions_buffer()); check_gl
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 
			(void*)(0)); check_gl
		
		glEnableVertexAttribArray(1); check_gl
		glBindBuffer(GL_ARRAY_BUFFER, normals_buffer()); check_gl
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
			(void*)(0)); check_gl
		
		glEnableVertexAttribArray(2); check_gl
		glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer()); check_gl
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
			(void*)(0));
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer()); check_gl
		
		glDrawElements((GLenum)dt, (oindex_count == -1 ? idxcnt : oindex_count), 
			GL_UNSIGNED_SHORT, (void*)0); check_gl
	}

	mesh_psnmtx::~mesh_psnmtx()
	{
		glDeleteBuffers(4, bufid);
	}

	//template <typename vertex_type, typename index_type>
	//mesh<vertex_type, index_type>::mesh(device* _dev, const string& n, const vector<vertex_type>& v, const vector<index_type>& i)
	//	: _name(n), vertices(v), indices(i)
	//{
	//	glGenVertexArrays(1, &vtx_array);
	//	glBindVertexArray(vtx_array);
	//	glGenBuffers(2, bufid);
	//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer());
	//	glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(vertex_type), &v[0], GL_STATIC_DRAW);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer());
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size()*sizeof(index_type), &i[0], GL_STATIC_DRAW);
	//}

	//template <typename vertex_type, typename index_type>
	//void mesh<vertex_type, index_type>::draw(device* _dev, prim_draw_type drawtype)
	//{
	//	glBindVertexArray(vtx_array);
	//	glEnableVertexAttribArray(0);
	//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer());
	//	gl_vertex_attrib* vtx_attrib = vertex_type::get_vertex_attribs();
	//	for (int i = 0; i < ARRAYSIZE(vtx_attrib); ++i)
	//	{
	//		glEnableVertexAttribArray(i);
	//		vtx_attrib[i].apply();
	//	}

	//	glDrawElements(drawtype, indices.size(), format_for_index_type<index_type>::format, (void*)0);
	//}

	//template <typename vertex_type, typename index_type>
	//mesh<vertex_type, index_type>::~mesh()
	//{
	//	glDeleteVertexArrays(1, &vtx_array);
	//	glDeleteBuffers(2, bufid);
	//}

#endif
}

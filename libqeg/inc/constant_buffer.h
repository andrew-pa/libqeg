#pragma once
#include "cmmn.h"
#include "device.h"
#include "shader.h"

namespace qeg
{
	template <typename T>
	class constant_buffer
	{
		T _data;
		bool changed;		
		uint _slot;

#ifdef DIRECTX
		ComPtr<ID3D11Buffer> _buf;
		shader_stage _ss;
#endif
#ifdef OPENGL
		GLuint _buf;
		GLuint _ix;
		GLuint _gi;
		string block_name;
		device* _alloc_dev;

		const GLchar* generate_block_name(int slot, shader_stage shs)
		{
			GLchar* nm = new GLchar[32];
			GLchar* shss = new GLchar[8];
			if(shs == shader_stage::vertex_shader)
				shss = "vs";
			else if(shs == shader_stage::pixel_shader)
				shss = "ps";
			sprintf(nm, "%s_block_%i", shss, slot);
			return nm;
		}
#endif
	public:
		constant_buffer(device* _dev, const shader& sh, uint slot,  T d = T(), shader_stage shs = shader_stage::pixel_shader)
			: _data(d), _slot(slot), 
				changed(true)
#ifdef DIRECTX
			, _ss(shs)
		{
			CD3D11_BUFFER_DESC bd(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
			chr(_dev->ddevice()->CreateBuffer(&bd, nullptr, &_buf));
		}
#elif OPENGL
				, block_name(generate_block_name(slot, shs)), _alloc_dev(_dev)
		{
			glGenBuffers(1, &_buf); check_gl
			glBindBuffer(GL_UNIFORM_BUFFER, _buf); check_gl
			glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &_data, GL_STREAM_DRAW); check_gl
			_ix = glGetUniformBlockIndex(sh.program_id(), block_name.c_str()); check_gl
			if (_ix == GL_INVALID_INDEX)
				throw exception("glGetUniformBlockIndex returned a invalid index");
			_gi = _dev->alloc_ubbi();
			glBindBufferRange(GL_UNIFORM_BUFFER, _gi, _buf, 0, sizeof(T)); check_gl
			glUniformBlockBinding(sh.program_id(), _ix, _gi); check_gl
				
		}
#endif
		~constant_buffer()
#ifdef DIRECTX 
		{}
#elif OPENGL
		{
			if (_buf == 0) return;
			_alloc_dev->free_ubbi(_gi);
			glDeleteBuffers(1, &_buf);
		}
#endif
		
		inline const T& get_data() { return _data; }
		inline T& data() { changed = true; return _data; }
		inline void data(T& d) { changed = true; _data = d; }

		void bind(device* _dev)
#ifdef DIRECTX 
		{
			switch (_ss)
			{
			case qeg::shader_stage::pixel_shader:
				_dev->context()->PSSetConstantBuffers(_slot, 1, _buf.GetAddressOf());
				break;
			case qeg::shader_stage::vertex_shader:
				_dev->context()->VSSetConstantBuffers(_slot, 1, _buf.GetAddressOf());
				break;
			case qeg::shader_stage::geometry_shader:
				_dev->context()->GSSetConstantBuffers(_slot, 1, _buf.GetAddressOf());
				break;
			}
		}
#elif OPENGL
		{
		}
#endif
		void bind(device* _dev, shader& ov_shd)
#ifdef DIRECTX 
		{
			bind(_dev);
		}
#elif OPENGL
		{
			v_ix = glGetUniformBlockIndex(sh.program_id(), block_name); check_gl
			if (v_ix == GL_INVALID_INDEX)
				throw exception("glGetUniformBlockIndex returned a invalid index");
			glUniformBlockBinding(sh.program_id(), v_ix, _gi); check_gl
		}
#endif

		void unbind(device* _dev)
#ifdef DIRECTX 
		{				
			ID3D11Buffer* nb[] = { nullptr };
			switch (_ss)
			{
			case qeg::shader_stage::pixel_shader:
				_dev->context()->PSSetConstantBuffers(_slot, 1, nb);
				break;
			case qeg::shader_stage::vertex_shader:
				_dev->context()->VSSetConstantBuffers(_slot, 1, nb);
				break;
			case qeg::shader_stage::geometry_shader:
				_dev->context()->GSSetConstantBuffers(_slot, 1, nb);
				break;
			}
		}
#elif OPENGL
		{			
			//should something happen here for the bind(_dev, ov_shd) case?
		}
#endif

		void update(device* _dev)
#ifdef DIRECTX 
		{
			if(!changed) return;
			changed = false;
			_dev->context()->UpdateSubresource(_buf.Get(), 0, nullptr, &_data, 0, 0);
		}
#elif OPENGL
		{
			if (!changed) return;
			changed = false;
			glBindBuffer(GL_UNIFORM_BUFFER, _buf); check_gl
				glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &_data, GL_DYNAMIC_DRAW); check_gl
		}
#endif

		void set_slot(uint slot, const shader& sh, shader_stage shs = shader_stage::pixel_shader)
		{
			_slot = slot;
#ifdef DIRECTX
			_ss = shs;
#elif OPENGL
			glBindBuffer(GL_UNIFORM_BUFFER, _buf);
			_ix = glGetUniformBlockIndex(sh.program_id(), generate_block_name(slot, shs));
			if (_ix == GL_INVALID_INDEX)
				throw exception("glGetUniformBlockIndex returned a invalid index");
			//_gi = rand() % 75;
			glBindBufferRange(GL_UNIFORM_BUFFER, _gi, _buf, 0, sizeof(T));
			glUniformBlockBinding(sh.program_id(), _ix, _gi); 
#endif
		}

#ifdef DIRECTX
		propr(uint, slot, { return _slot; });
		propr(ComPtr<ID3D11Buffer>, buffer, { return _buf; });
#endif
	};
}


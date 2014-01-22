#include "shader.h"

namespace qeg
{
#ifdef DIRECTX
	shader::shader(device* _dev, datablob<byte>* vs_data, datablob<byte>* ps_data)
	{
		if (vs_data != nullptr)
			chr(_dev->ddevice()->CreateVertexShader(vs_data->data, vs_data->length, nullptr, &vertex_sh));
		if (ps_data != nullptr)
			chr(_dev->ddevice()->CreatePixelShader(ps_data->data, ps_data->length, nullptr, &pixel_sh));
	}

	void shader::bind(device* _dev)
	{
		_dev->context()->VSSetShader(vertex_sh.Get(), nullptr, 0);
		_dev->context()->PSSetShader(pixel_sh.Get(), nullptr, 0);
	}

	void shader::unbind(device* _dev)
	{
		_dev->context()->VSSetShader(nullptr, nullptr, 0);
		_dev->context()->PSSetShader(nullptr, nullptr, 0);
	}

	void shader::update(device* _dev)
	{
	}

	shader::~shader()
	{
	}
#endif
#ifdef OPENGL
	static void validate_shader(GLuint shader, const string& fn)
	{
		char buf[512];
		GLsizei len = 0;
		glGetShaderInfoLog(shader, 512, &len, buf);
		if (len > 0)
		{
			ostringstream oss;
			oss << "Shader " << shader << " (" << fn << ")" << " error: " << buf;
			throw exception(oss.str().c_str());
		}
	}
	shader::shader(device* _dev, datablob<byte>* vs_data, datablob<byte>* ps_data)
	{
		if(vs_data != nullptr)
		{
			_idvp = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(_idvp, 1, (GLchar**)&vs_data->data, 0);
			glCompileShader(_idvp);
			validate_shader(_idvp, (GLchar*)ps_data->data);
		}

		if(ps_data != nullptr)
		{
			glShaderSource(_idfp, 1, (GLchar**)&ps_data->data, 0);
			glCompileShader(_idfp);
			validate_shader(_idfp, (GLchar*)ps_data->data);
			_idfp = glCreateShader(GL_FRAGMENT_SHADER);
		}

		_id = glCreateProgram();
		if(vs_data != nullptr)
			glAttachShader(_id, _idvp);
		if(ps_data != nullptr)
			glAttachShader(_id, _idfp);
		
		glLinkProgram(_id);

		{
			char buf[512];
			GLsizei len = 0;
			glGetProgramInfoLog(_id, 512, &len, buf);
			if (len > 0)
			{
				OutputDebugString(L"GL Program error: ");
				OutputDebugStringA(buf);
				OutputDebugString(L"\n");
				throw exception(buf);
			}
			glValidateProgram(_id);
			GLint sta;
			glGetProgramiv(_id, GL_VALIDATE_STATUS, &sta);
			if (sta == GL_FALSE) throw exception("error validating shader");
		}

	}

	void shader::bind(device* _dev)
	{
		glUseProgram(_id);
	}

	void shader::unbind(device* _dev)
	{
		glUseProgram(0);
	}

	void shader::update(device* _dev)
	{
	}

	shader::~shader()
	{
		glDetachShader(_id, _idvp);
		glDetachShader(_id, _idfp);

		glDeleteShader(_idfp);
		glDeleteShader(_idvp);
		glDeleteProgram(_id);
	}
#endif
}

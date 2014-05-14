#define LINK
#define DIRECTX 88
#include "cmmn.h"
#include "bo_file.h"
#include "resource.h"

#include "gli/gli.hpp"

//txcv:	 reads in textures (DDS) and converts them into .tex files
//usage txcv <in_file> <out_file>

//convert gli::format to pi_pixel_format
#undef RGB
#define RM pi_pixel_format::
#define pfb(bw, type) RM R##bw##_##type, \
	RM RG##bw##_##type, \
	RM RGB##bw##_##type, \
	RM RGBA##bw##_##type, \
	
qeg::detail::pi_pixel_format convert_pi(gli::format f)
{
	using qeg::detail::pi_pixel_format;
	using gli::format;
	pi_pixel_format conv_table[] = 
	{
		RM UNKNOWN,
		
		pfb(8, UNORM)
		pfb(16, UNORM)
		pfb(32, UNORM)

		pfb(8, SNORM)
		pfb(16, SNORM)
		pfb(32, SNORM)

		pfb(8, UINT)
		pfb(16, UINT)
		pfb(32, UINT)

		pfb(8, SINT)
		pfb(16, SINT)
		pfb(32, SINT)

		pfb(16, FLOAT)
		pfb(32, FLOAT)

		//depth formats
		//compressed formats
		//sRGB formats
	};
	return conv_table[(uint)f];
}

qeg::bo_file* write_tex(gli::storage& s)
{
	if(s.layers() > 1)
	{
		throw exception("texture arrays not yet supported");
	}
	else
	{
	}
}

int main(int argc, char* argv[])
{
	vector<string> args;
	for (int i = 1; i < argc; ++i) args.push_back(argv[i]);
	
	string in_file = "test.dds";//args[0];
	string out_file = "test.tex";//args[1];

	auto i = gli::load_dds(in_file.c_str());

	/*auto bf = write_tex(i);
	auto d = bf->write();
	ofstream out(out_file, ios_base::binary);
	out.write((const char*)d.data, d.length);
	out.flush();
	out.close();*/

	if(i.layers() > 1) //texture array
	{
		throw exception("texture arrays not yet supported");

		if (i.levels() > 1)
		{
			if (i.dimensions(0).y == 1) //1d texture array
			{
				gli::texture1DArray t(i);

			}
			else if (i.dimensions(0).z == 1) //2d texture array
			{
				gli::texture2DArray t(i);
			}
		}
		else
		{
			if (i.dimensions(0).y == 1) //1d texture array
			{
				gli::texture1DArray t(i);

			}
			else if (i.dimensions(0).z == 1) //2d texture array
			{
				gli::texture2DArray t(i);
			}
		}
	}
	else if(i.faces() > 1) //texture cube
	{
		gli::textureCube t(i);
		if(i.levels() > 1)
		{
		}
		else
		{
		}
	}
	else //regular texture
	{
		if(i.levels() > 1)
		{
			if (i.dimensions(0).y == 1) //1d texture
			{
				gli::texture1D t(i);

			}
			else if (i.dimensions(0).z == 1) //2d texture
			{
				gli::texture2D t(i);
			}
			else //3d texture
			{
				gli::texture3D t(i);
			}
		}
		else
		{
			if(i.dimensions(0).y == 0) //1d texture
			{
				gli::texture1D t(i);
				auto td = t.data();

				qeg::detail::texture_header hed;
				hed.dim = qeg::texture_dimension::texture_1d;
				hed.size = (uvec3)t.dimensions();
				hed.mip_count = 0;
				hed.array_count = 0;
				hed.format = convert_pi(t.format());
				
				qeg::bo_file texf(qeg::bo_file::file_type::texture);
				qeg::bo_file::chunk hedc(0,
					new qeg::datablob<glm::byte>((glm::byte*)&hed, sizeof(qeg::detail::texture_header)));
				qeg::bo_file::chunk dc(1,
					new qeg::datablob<glm::byte>((glm::byte*)t.data(), t.size()));
				auto txf = texf.write();
				ofstream out(out_file, ios_base::binary);
				out.write((const char*)txf.data, txf.length);
				out.flush();
				out.close();
			}
			else if(i.dimensions(0).z == 1) //2d texture
			{
				auto t = i;
				auto pif = convert_pi(t.format());
				auto td = t.data();

				qeg::detail::texture_header hed;
				hed.dim = qeg::texture_dimension::texture_2d;
				hed.size = uvec3(t.dimensions(0));
				hed.mip_count = 0;
				hed.array_count = 0;
				hed.format = pif;

				qeg::bo_file texf(qeg::bo_file::file_type::texture);
				qeg::bo_file::chunk hedc(0,
					new qeg::datablob<glm::byte>((glm::byte*)&hed, sizeof(qeg::detail::texture_header)));
				qeg::bo_file::chunk dc(1,
					new qeg::datablob<glm::byte>((glm::byte*)t.data(), t.size()));// *qeg::bytes_per_pixel(qeg::detail::convert(pif))));
				texf.chunks().push_back(hedc);
				texf.chunks().push_back(dc);
				auto txf = texf.write();
				ofstream out(out_file, ios_base::binary);
				out.write((const char*)txf.data, txf.length);
				out.flush();
				out.close();
			}
			else //3d texture
			{
				gli::texture3D t(i);
			}
		}
	}
	
	getchar();

	return 0;
}
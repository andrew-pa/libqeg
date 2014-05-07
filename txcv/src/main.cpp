#define LINK
#define DIRECTX 88
#include "cmmn.h"
#include "bo_file.h"
#include "resource.h"

namespace qeg { namespace detail {
	pixel_format convert(pi_pixel_format f) { return pixel_format::UNKNOWN;  }

	pi_pixel_format convert(pixel_format f) { return pi_pixel_format::UNKNOWN; }

	void* _load_texture(device* dev, datablob<byte>* file_data) { return nullptr; }
}}

//txcv:	 reads in textures and converts them into .tex files

int main()
{
	
	getchar();

	return 0;
}
#pragma once

#include "..\\Common\\variable.h"

#include "CRC32.h"

struct IHDR{
	UINT width;
	UINT height;
	BYTE bit_depth;
	BYTE color_type;
	BYTE compress_type;
	BYTE filter_type;
	BYTE interlace_type;
};

union b2i{
	BYTE b[4];
	UINT i;
};

bool IHDRChunk(BYTE* buf, INT& pos, IHDR& ihdr);
bool PLTEChunk(BYTE* buf, INT& pos, UINT*& palette, UINT& psize);
bool IDATChunk(BYTE* buf, INT& pos, BYTE*& cbuf, UINT& csize, UINT& cpos);
bool IENDChunk(BYTE* buf, INT& pos);
bool tRNSChunk(BYTE* buf, INT& pos, BYTE color_type, UINT*& palette, UINT palette_size);
bool DefaultChunk(BYTE* buf, INT& pos);
bool Draw(IMAGE& img, IHDR ihdr, BYTE* ucbuf, UINT ucsize, UINT* palette, UINT palette_size);
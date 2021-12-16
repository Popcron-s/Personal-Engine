#pragma once

#include "..\\Common\\variable.h"

bool ZIPDecoder(BYTE* buf, UINT size, UINT*& LH_offset, UINT& LH_size);
bool Decompression(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name, BYTE*& decomp_buf, UINT& decomp_size);
bool GetFilename(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name);
#pragma once

#include "..\\Common\\variable.h"

bool TrueTypeDecoder(const BYTE* buf, const UINT size, 
					 BYTE*& cmap_buf, UINT& cmap_size,
					 BYTE*& glyf_buf, UINT& glyf_size,
					 BYTE*& hmtx_buf, UINT& hmtx_size,
					 UINT*& glyphOffsets, UINT& numHMat,
					 GLYPH*& glyph, UINT& glyph_size,
					 INT& min_x, INT& max_x, INT& min_y, INT& max_y);
bool TrueTypeGlyph(const BYTE* cmap_buf, const UINT cmap_size, 
				   const BYTE* glyf_buf, const UINT glyf_size,
				   const BYTE* hmtx_buf, const UINT hmtx_size,
				   const UINT* glyphOffsets, const UINT numHMat,
				   UINT word, GLYPH*& glyph_array, GLYPH*& glyph_output);
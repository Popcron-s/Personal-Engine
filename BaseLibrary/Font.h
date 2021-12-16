#pragma once

#include "..\\Common\\variable.h"

class Font{
private:
	BYTE* cmap_buf;
	UINT cmap_size;

	BYTE* glyf_buf;
	UINT glyf_size;

	BYTE* hmtx_buf;
	UINT hmtx_size;

	UINT* glyphOffsets;
	UINT numHMat;

	GLYPH* glyph;
	UINT glyph_size;	//numGlyph
	//glyf_buf->insert glyph struct
	INT min_x;
	INT max_x;
	INT min_y;
	INT max_y;
	//advance height -> max_y-min_y

public:
	Font();
	~Font();

	bool CreateFont(BYTE*, UINT);
	bool DeleteFont();

	//bool GetGlyphImage(wchar_t, IMAGE&){}
	bool GetGlyph(UINT, GLYPH*&);

	//bool GetStringImage(wchar_t*, IMAGE&){}
	//bool GetString(wchar_t*, GLYPH*&){}

	UINT16 GetMaxWidth();	//using spase
	UINT16 GetSpace();
	INT GetMinY();
	INT GetMaxY();
	UINT GetVerticalAdvance();
};
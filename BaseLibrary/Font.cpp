#include "Font.h"
#include "..\\Coder\\TrueTypeDecoder.h"
#include "..\\Interface\\Base_FileIO.h"

Font::Font() : 
	cmap_buf(nullptr), cmap_size(0), 
	glyf_buf(nullptr), glyf_size(0), 
	hmtx_buf(nullptr), hmtx_size(0), 
	glyphOffsets(nullptr), numHMat(0), glyph(nullptr), glyph_size(0),
	min_x(0), max_x(0), min_y(0), max_y(0){}
Font::~Font(){DeleteFont();}

bool Font::CreateFont(BYTE* _buf, UINT _size){
	DeleteFont();
	return TrueTypeDecoder(_buf, _size, 
						   cmap_buf, cmap_size,
						   glyf_buf, glyf_size,
						   hmtx_buf, hmtx_size,
						   glyphOffsets, numHMat,
						   glyph, glyph_size, 
						   min_x, max_x, min_y, max_y);
}
bool Font::DeleteFont(){
	if(cmap_buf != nullptr){
		delete[] cmap_buf;
		cmap_buf = nullptr;
	}
	cmap_size = 0;

	if(glyf_buf != nullptr){
		delete[] glyf_buf;
		glyf_buf = nullptr;
	}
	glyf_size = 0;

	if(hmtx_buf != nullptr){
		delete[] hmtx_buf;
		hmtx_buf = nullptr;
	}
	hmtx_size = 0;

	if(glyphOffsets != nullptr){
		delete[] glyphOffsets;
		glyphOffsets = nullptr;
	}
	numHMat = 0;

	if(glyph != nullptr){
		for(INT i = 0 ; i<glyph_size ; ++i){
			if(glyph[i].buf != nullptr){delete[] glyph[i].buf;}
		}
		delete[] glyph;
		glyph = nullptr;
	}
	glyph_size = 0;

	min_x = 0;	max_x = 0;
	min_y = 0;	max_y = 0;

	return true;
}

bool Font::GetGlyph(UINT word, GLYPH*& _glyph){
	//if(((word) & 0xD800) == 0xD800){return false;}	//0xD800 and 0xDC00 is fair utf-32
	//if(glyphOffsets == nullptr){std::cout<<"font dont save glyphOffsets"<<std::endl;}
	UINT idx = 0;
	if(!TrueTypeGlyph(cmap_buf, cmap_size, 
					  glyf_buf, glyf_size,
					  hmtx_buf, hmtx_size,
					  glyphOffsets, numHMat,
					  word, glyph, _glyph)){return false;}
	return true;
}

UINT16 Font::GetMaxWidth(){return max_x-min_x;}
UINT16 Font::GetSpace(){return (max_x-min_x)*0.25f;}
INT Font::GetMinY(){return min_y;}
INT Font::GetMaxY(){return max_y;}
UINT Font::GetVerticalAdvance(){return max_y-min_y;}
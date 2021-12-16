#include "PNGDecoder.h"

#include "ChunkReader.h"

#include "Inflate.h"
#include "Adler32.h"

bool PNGDecoder(BYTE* buf, UINT size, IMAGE& image_data){
	if(buf == nullptr){return false;}
	if(buf[0] != 0x89 || buf[1] != 0x50 || buf[2] != 0x4E || buf[3] != 0x47 || 
		buf[4] != 0x0D || buf[5] != 0x0A || buf[6] != 0x1A || buf[7] != 0x0A ){
		return false;
	}

	IHDR ihdr = {};

	UINT* palette = nullptr;
	UINT palette_size = 0;

	BYTE* ucbuf = nullptr;
	UINT ucsize = 0;	//uc = uncompress
	UINT ucpos = 0;

	BYTE* cbuf = nullptr;
	UINT csize = 0;
	UINT cpos = 0;

	b2i chunk_name;
#define s2i(s) (s[0] | (s[1] << 8) | (s[2] << 16) | (s[3] << 24))
	for(INT i = 8 ; i < (size-3) ; ++i){
		chunk_name.b[0] = buf[i+0]; chunk_name.b[1] = buf[i+1];
		chunk_name.b[2] = buf[i+2]; chunk_name.b[3] = buf[i+3];
		//TEXT type = ctype;

		switch(chunk_name.i){
			//Critical chunks
		case s2i("IHDR") :
			if(!IHDRChunk(buf, i, ihdr)){return false;}
			ucsize = (ihdr.width+1)*ihdr.height*4;
			ucbuf = new BYTE[ucsize];
			for(UINT i = 0 ; i<ucsize ; ++i){ucbuf[i] = 0x00;}
			csize = (ihdr.width+1)*ihdr.height*4;
			cbuf = new BYTE[csize];
			for(UINT i = 0 ; i<csize ; ++i){cbuf[i] = 0x00;}
			ucpos = 0;
			break;
		case s2i("PLTE") :
			if(!PLTEChunk(buf, i, palette, palette_size)){return false;}
			break;
		case s2i("IDAT") :
			if(!IDATChunk(buf, i, cbuf, csize, cpos)){return false;}
			break;
		case s2i("IEND") :
			if(!IENDChunk(buf, i)){return false;}
			break;
			//Ancillary chunks
			//Transparency Info.
		case s2i("tRNS") :
			if(!tRNSChunk(buf, i, ihdr.color_type, palette, palette_size)){return false;}
			break;
			//Color Space Info.
		case s2i("cHRM") : 
		case s2i("gAMA") :
		case s2i("iCCP") :
		case s2i("sBIT") :
		case s2i("sRGB") :
			//Text Info.
		case s2i("iTXt") :
		case s2i("tEXt") :
		case s2i("zTXt") :
			//etc... Info.
		case s2i("bkGD") :
		case s2i("hIST") :
		case s2i("pHYs") :
		case s2i("sPLT") :
			//Time Info
		case s2i("tTME") :
			if(!DefaultChunk(buf, i)){return false;}
			break;
		}
	}
#undef s2i

	if(cpos >= csize){return false;}
	csize = cpos;
	cpos = 0;
	if(!DataUncompress(cbuf, csize, cpos, ucbuf, ucsize, ucpos)){return false;}
	if(ucpos < ucsize){ucsize = ucpos;}
	if(!Draw(image_data, ihdr, ucbuf, ucsize, palette, palette_size)){return false;}
	return true;
}
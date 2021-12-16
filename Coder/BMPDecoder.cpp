#include "BMPDecoder.h"

bool BMPDecoder(BYTE* buf, UINT size, IMAGE& image_data){
	if(buf == nullptr){return false;}
	if(buf[0] != 0x42 || buf[1] != 0x4D){return false;}
#define little_b2i(a,b,c,d) ((a) | (b<<8) | (c<<16) | (d<<24))
	UINT offset = little_b2i(buf[10], buf[11], buf[12], buf[13]);

	UINT width = 0, height = 0, bit_depth = 0;

	switch(little_b2i(buf[14],buf[15],buf[16],buf[17])){
	case 40:	//BITMAPINFOHEADER
		width = little_b2i(buf[18],buf[19],buf[20],buf[21]);
		height = little_b2i(buf[22],buf[23],buf[24],buf[25]);
		bit_depth = little_b2i(buf[28],buf[29],0,0);
		if(little_b2i(buf[30],buf[31],buf[32],buf[33]) != 0x00){return false;}
		break;
	case 12:
	case 64:
	case 108:
	case 124:
	default: return false;
	}
#undef little_b2i
	UINT wmb = (width*bit_depth)/8;

	BYTE* image_buf = &(buf[offset]);
	image_data.width = width;
	image_data.height = height;
	image_data.bit_depth = bit_depth;
	image_data.alpha_able = false;
	image_data.buf = new BYTE[wmb*height];

	UINT pass_byte = wmb%4;
	pass_byte = (pass_byte>0)?4-pass_byte:0;

	UINT i = 0;
	for(UINT y = height ; y>0 ; --y){
		for(UINT x = 0 ; x<width ; ++x){
			for(UINT bit = bit_depth/8 ; bit>0 ; --bit){
				image_data.buf[((y-1)*wmb)+(x*(bit_depth/8))+(bit-1)] = image_buf[i];
				++i;
			}
		}
		i += pass_byte;
	}

	return true;
}
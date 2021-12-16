#include "TextDecoder.h"

bool UTF8(BYTE* buf, UINT size, wchar_t*& text, UINT& len){
	//0xxx xxxx
	//110x xxxx 10xx xxxx
	//1110 xxxx 10xx xxxx 10xx xxxx
	//1111 0zzz 10zz xxxx 10xx xxxx 10xx xxxx	//utf-8
	//1101 10yy yyxx xxxx 1101 11xx xxxx xxxx	//utf-16
	
	for(UINT i = 3 ; i<size ; ){
		if((buf[i+0]&0x80) == 0x00){i+=1; ++len; continue;}
		if((buf[i+0]&0xE0) == 0xC0){
			if((buf[i+1]&0xC0) == 0x80){i+=2; ++len; continue;}
		}
		if((buf[i+0]&0xF0) == 0xE0){
			if((buf[i+1]&0xC0) == 0x80){
				if((buf[i+2]&0xC0) == 0x80){i+=3; ++len; continue;}
			}
		}
		if((buf[i+0]&0xF8) == 0xF0){
			if((buf[i+1]&0xC0) == 0x80){
				if((buf[i+2]&0xC0) == 0x80){
					if((buf[i+3]&0xC0) == 0x80){i+=4; len+=2; continue;}
				}
			}
		}
	}

	text = new wchar_t[len+1];
	text[len] = 0x00;

	for(UINT i = 3, idx = 0 ; i<size ; ){
		wchar_t c = 0;
		if((buf[i]&0x80) == 0x00){
			c |= buf[i++];
			text[idx++] = c;
			continue;
		}
		if((buf[i]&0xE0) == 0xC0){
			c |= (buf[i++] & 0x1F)<<6;
			if((buf[i]&0xC0) == 0x80){
				c |= buf[i++] & 0x3F;
				text[idx++] = c;
				continue;
			}
		}
		if((buf[i]&0xF0) == 0xE0){
			c |= (buf[i++] & 0x0F)<<12;
			if((buf[i]&0xC0) == 0x80){
				c |= (buf[i++] & 0x3F)<<6;
				if((buf[i]&0xC0) == 0x80){
					c |= buf[i++] & 0x3F;
					text[idx++] = c;
					continue;
				}
			}
		}
		if((buf[i]&0xF8) == 0xF0){
			c |= 0xD800;
			UINT y = 0;
			y |= (buf[i++]&0x07)<<2;
			if((buf[i]&0xC0) == 0x80){
				y |= (buf[i]&0x30);
				c |= (buf[i++]&0x0F)<<2;
				if((buf[i]&0xC0) == 0x80){
					c |= (buf[i]&0x30)>>4;
					y -= 1;
					c |= y<<6;
					text[idx++] = c;
					c = 0xDC00;
					c |= (buf[i++]&0x0F)<<6;
					if((buf[i]&0xC0) == 0x80){
						c |= buf[i++]&0x3F;
						text[idx++] = c;
						continue;
					}
				}
			}
		}
	}
	return true;
}
//BE = big endian / LE = little endian
bool UNICODE_LE(BYTE* buf, UINT size, wchar_t*& text, UINT& len){return false;}

bool UNICODE_BE(BYTE* buf, UINT size, wchar_t*& text, UINT& len){return false;}

bool ANSI(BYTE* buf, UINT size, wchar_t*& text, UINT& len){return false;}

bool TXTDecoder(BYTE* buf, UINT size, wchar_t*& text, UINT& len){
	if(text != nullptr){
		delete[] text;
		text = nullptr;
	}
	len = 0;

	if(size >= 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF){return UTF8(buf, size, text, len);}
	else if(size >= 2 && buf[0] == 0xFE && buf[1] == 0xFF){return UNICODE_BE(buf, size, text, len);}
	else if(size >= 2 && buf[0] == 0xFF && buf[1] == 0xFE){return UNICODE_LE(buf, size, text, len);}
	else{return ANSI(buf, size, text, len);}
}
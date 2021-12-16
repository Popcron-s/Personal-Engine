#include "ChunkReader.h"
#include "CRC32.h"

bool IHDRChunk(BYTE* buf, INT& pos, IHDR& ihdr){
	pos -= 4;
	b2i temp = {};
	temp.b[3] = buf[pos]; temp.b[2] = buf[pos+1]; temp.b[1] = buf[pos+2]; temp.b[0] = buf[pos+3];
	UINT size = temp.i;
	//+4 +5 +6 +7 name
	temp.b[3] = buf[pos+8]; temp.b[2] = buf[pos+9]; temp.b[1] = buf[pos+10]; temp.b[0] = buf[pos+11];
	ihdr.width = temp.i;
	temp.b[3] = buf[pos+12]; temp.b[2] = buf[pos+13]; temp.b[1] = buf[pos+14]; temp.b[0] = buf[pos+15];
	ihdr.height = temp.i;
	ihdr.bit_depth = buf[pos+16];
	ihdr.color_type = buf[pos+17];
	ihdr.compress_type = buf[pos+18];
	ihdr.filter_type = buf[pos+19];
	ihdr.interlace_type = buf[pos+20];

	if(!CRC32(&(buf[pos]), 12+size)){return false;}

	pos += 12+size;

	return true;
}

bool PLTEChunk(BYTE* buf, INT& pos, UINT*& palette, UINT& psize){
	pos -= 4;
	b2i temp = {};
	temp.b[3] = buf[pos]; temp.b[2] = buf[pos+1]; temp.b[1] = buf[pos+2]; temp.b[0] = buf[pos+3];
	UINT size = temp.i;

	if(size%3){return false;}
	psize = size/3;
	if(palette != nullptr){
		delete [] palette;
		palette = new UINT[psize];
	}
	for(UINT i = 0 ; i<psize ; ++i){
		palette[i] = buf[pos+8+(i*3)] | (buf[pos+8+(i*3)+1]<<8) | (buf[pos+8+(i*3)+2]<<16) | (0xFF<<24);
	}

	if(!CRC32(&(buf[pos]), 12+size)){return false;}
	pos += 12+size;
	return true;
}

bool IDATChunk(BYTE* buf, INT& pos, BYTE*& cbuf, UINT& csize, UINT& cpos){
	pos -= 4;
	b2i temp = {};
	temp.b[3] = buf[pos]; temp.b[2] = buf[pos+1]; temp.b[1] = buf[pos+2]; temp.b[0] = buf[pos+3];
	UINT size = temp.i;

	if(csize == 0 || cbuf == nullptr){return false;}
	for(INT i = 0 ; i<size ; ++i){cbuf[cpos++] = buf[pos+8+i];}

	if(!CRC32(&(buf[pos]), 12+size)){return false;}
	pos += 12+size;
	return true;
}

bool IENDChunk(BYTE* buf, INT& pos){
	pos -= 4;
	if(!CRC32(&(buf[pos]), 12)){return false;}
	pos += 12;
	return true;
}

bool tRNSChunk(BYTE* buf, INT& pos, BYTE color_type, UINT*& palette, UINT palette_size){
	pos -= 4;
	b2i temp = {};
	temp.b[3] = buf[pos]; temp.b[2] = buf[pos+1]; temp.b[1] = buf[pos+2]; temp.b[0] = buf[pos+3];
	UINT size = temp.i;

	if(color_type == 3){	//palette
		if(palette_size != size){return false;}

		for(UINT i = 0 ; i<palette_size ; ++i){
			palette[i] |= (buf[pos+8+i]<<24);
		}
	}

	if(!CRC32(&(buf[pos]), 12+size)){return false;}
	return true;
}

bool DefaultChunk(BYTE* buf, INT& pos){
	pos -= 4;
	b2i temp = {};
	temp.b[3] = buf[pos]; temp.b[2] = buf[pos+1]; temp.b[1] = buf[pos+2]; temp.b[0] = buf[pos+3];
	UINT size = temp.i;
	if(!CRC32(&(buf[pos]), 12+size)){return false;}
	pos += 12+size;
	return true;
}

bool Draw(IMAGE& img, IHDR ihdr, BYTE* ucbuf, UINT ucsize, UINT* palette, UINT palette_size){
	if(ihdr.filter_type != 0){return false;}

	img.width = ihdr.width;
	img.height = ihdr.height;
	img.alpha_able = false;
	img.bit_depth = 0;
	img.buf = nullptr;

	switch(ihdr.color_type){
	case 0x00:	//gray
		break;	
	case 0x02:	//true color
	{
		UINT wmb = ihdr.width*(ihdr.bit_depth/8)*3;
		UINT max_size = wmb*ihdr.height;

		img.alpha_able = false;
		img.bit_depth = ihdr.bit_depth*3;
		img.buf = new BYTE[max_size];

		for(UINT imgpos = 0, ucpos = 0 ; ucpos<ucsize ; ){
			UINT filter = ucbuf[ucpos++];
			switch(filter){
			case 0x00:
				for(UINT j = 0 ; j<wmb ; ++j){
					img.buf[imgpos++] = ucbuf[ucpos++];
				}
				break;
			case 0x01:
				img.buf[imgpos++] = ucbuf[ucpos++];
				img.buf[imgpos++] = ucbuf[ucpos++];
				img.buf[imgpos++] = ucbuf[ucpos++];
				for(UINT j = 3 ; j<wmb ; ++j){
					UINT t = ucbuf[ucpos++]+img.buf[imgpos-3];
					img.buf[imgpos++] = t&0xFF;
				}
				break;
			case 0x02:
				if(imgpos<wmb){
					for(UINT j = 0 ; j<wmb ; ++j){
						img.buf[imgpos++] = ucbuf[ucpos++];
					}
				}
				else{
					for(UINT j = 0 ; j<wmb ; ++j){
						UINT t = ucbuf[ucpos++]+img.buf[imgpos-wmb];
						img.buf[imgpos++] = t&0xFF;
					}
				}
				break;
			case 0x03:
				if(imgpos<wmb){
					img.buf[imgpos++] = ucbuf[ucpos++];
					img.buf[imgpos++] = ucbuf[ucpos++];
					img.buf[imgpos++] = ucbuf[ucpos++];
					for(UINT j = 3 ; j<wmb ; ++j){
						UINT t = ucbuf[ucpos++]+(img.buf[imgpos-3]/2);
						img.buf[imgpos++] = t&0xFF;
					}
				}
				else{
					for(UINT j = 0 ; j<3 ; ++j){
						UINT t = ucbuf[ucpos++]+(img.buf[imgpos-wmb]/2);
						img.buf[imgpos++] = t&0xFF;
					}
					for(UINT j = 3 ; j<wmb ; ++j){
						UINT t = ucbuf[ucpos++]+((img.buf[imgpos-wmb]+img.buf[imgpos-3])/2);
						img.buf[imgpos++] = t&0xFF;
					}
				}
				break;
			case 0x04:
				for(INT j = 0 ; j<wmb ; ++j){
					INT a = (j>=3)?img.buf[imgpos-3]:0;
					INT b = (imgpos>=wmb)?img.buf[imgpos-wmb]:0;
					INT c = ((j>=3) && (imgpos>=wmb))?img.buf[imgpos-wmb-3]:0;
					INT p = a+b-c;
					INT pa = p-a; pa = (pa<0)?-pa:pa;
					INT pb = p-b; pb = (pb<0)?-pb:pb;
					INT pc = p-c; pc = (pc<0)?-pc:pc;
					if(pa<=pb && pa<=pc){UINT t = ucbuf[ucpos++]+a; img.buf[imgpos++] = t&0xFF;}
					else if(pb<=pc)		{UINT t = ucbuf[ucpos++]+b; img.buf[imgpos++] = t&0xFF;}
					else				{UINT t = ucbuf[ucpos++]+c; img.buf[imgpos++] = t&0xFF;}
				}
				break;
			}
		}
	}
	break;
	case 0x03:	//palette
		break;
	case 0x04:	//alpha gray
		break;
	case 0x06:	//alpha true color
	{
		UINT wmb = ihdr.width*(ihdr.bit_depth/8)*4;
		UINT max_size = wmb*ihdr.height;

		img.alpha_able = true;
		img.bit_depth = ihdr.bit_depth*4;
		img.buf = new BYTE[max_size];

		for(UINT imgpos = 0, ucpos = 0 ; ucpos<ucsize ; ){
			UINT filter = ucbuf[ucpos++];
			switch(filter){
			case 0x00:
				for(UINT j = 0 ; j<wmb ; ++j){
					img.buf[imgpos++] = ucbuf[ucpos++];
				}
				break;
			case 0x01:
				img.buf[imgpos++] = ucbuf[ucpos++];
				img.buf[imgpos++] = ucbuf[ucpos++];
				img.buf[imgpos++] = ucbuf[ucpos++];
				img.buf[imgpos++] = ucbuf[ucpos++];
				for(UINT j = 4 ; j<wmb ; ++j){
					UINT t = ucbuf[ucpos++]+img.buf[imgpos-4];
					img.buf[imgpos++] = t&0xFF;
				}
				break;
			case 0x02:
				if(imgpos<wmb){
					for(UINT j = 0 ; j<wmb ; ++j){
						img.buf[imgpos++] = ucbuf[ucpos++];
					}
				}
				else{
					for(UINT j = 0 ; j<wmb ; ++j){
						UINT t = ucbuf[ucpos++]+img.buf[imgpos-wmb];
						img.buf[imgpos++] = t&0xFF;
					}
				}
				break;
			case 0x03:
				if(imgpos<wmb){
					img.buf[imgpos++] = ucbuf[ucpos++];
					img.buf[imgpos++] = ucbuf[ucpos++];
					img.buf[imgpos++] = ucbuf[ucpos++];
					img.buf[imgpos++] = ucbuf[ucpos++];
					for(UINT j = 4 ; j<wmb ; ++j){
						UINT t = ucbuf[ucpos++]+(img.buf[imgpos-4]/2);
						img.buf[imgpos++] = t&0xFF;
					}
				}
				else{
					for(UINT j = 0 ; j<4 ; ++j){
						UINT t = ucbuf[ucpos++]+(img.buf[imgpos-wmb]/2);
						img.buf[imgpos++] = t&0xFF;
					}
					for(UINT j = 4 ; j<wmb ; ++j){
						UINT t = ucbuf[ucpos++]+((img.buf[imgpos-wmb]+img.buf[imgpos-4])/2);
						img.buf[imgpos++] = t&0xFF;
					}
				}
				break;
			case 0x04:
				for(INT j = 0 ; j<wmb ; ++j){
					INT a = (j>=4)?img.buf[imgpos-4]:0;
					INT b = (imgpos>=wmb)?img.buf[imgpos-wmb]:0;
					INT c = ((j>=4) && (imgpos>=wmb))?img.buf[imgpos-wmb-4]:0;
					INT p = a+b-c;
					INT pa = p-a; pa = (pa<0)?-pa:pa;
					INT pb = p-b; pb = (pb<0)?-pb:pb;
					INT pc = p-c; pc = (pc<0)?-pc:pc;
					if(pa<=pb && pa<=pc){UINT t = ucbuf[ucpos++]+a; img.buf[imgpos++] = t&0xFF;}
					else if(pb<=pc)		{UINT t = ucbuf[ucpos++]+b; img.buf[imgpos++] = t&0xFF;}
					else				{UINT t = ucbuf[ucpos++]+c; img.buf[imgpos++] = t&0xFF;}
				}
				break;
			}
		}
	}
	break;
	}
	return true;
}
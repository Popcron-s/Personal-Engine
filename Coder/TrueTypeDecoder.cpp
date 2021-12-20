#include "TrueTypeDecoder.h"
#include "..\\BaseLibrary\\Font.h"

#include <iostream>

#define Output8(b, p) b[p++];
#define Output16(b, p) (b[p]<<8) | b[p+1]; p+=2;
#define Output24(b, p) (b[p]<<16) | (b[p+1]<<8) | b[p+2]; p+=3;
#define Output32(b, p) (b[p]<<24) | (b[p+1]<<16) | (b[p+2]<<8) | b[p+3]; p+=4;
#define Shift16(v) (((v&0x00FF)<<8) | ((v&0xFF00)>>8))

struct GLYF_DATA{
	UINT* contours;
	UINT contours_num;
	VECTOR2* point;
	//max point num = contours[contours_num-1]+1
};

bool SetGlyph(const BYTE* glyf_buf, const UINT glyf_size,
			  const BYTE* hmtx_buf, const UINT hmtx_size,
			  const UINT* glyphOffsets, const UINT numHMat,
			  const UINT index, GLYPH* glyph_array);

bool Checksum(const BYTE* buf, const UINT size, UINT checksum){
	UINT sum = 0;
	UINT* table = (UINT*)buf;
	UINT* end = table+((size+3)&~3)/4;
	while(table<end){sum += *(table++);}
	sum = 0xB1B0AFBA-sum;
	//std::cout<<std::hex<<checksum<<"/"<<sum<<"/"<<std::dec;
	if(sum == checksum){return true;}
	else{return false;}
}

bool TableChecksum(const BYTE* buf, const UINT size, UINT checksum){
	UINT sum = 0;
	UINT* table = (UINT*)buf;
	UINT* end = table+((size+3)&~3)/4;
	while(table<end){sum += *(table++);}
	if(sum == checksum){return true;}
	else{return false;}
}

void maxp(const BYTE* buf, const UINT size, UINT& numGlyphs){
	UINT pos = 0;
	UINT version = Output32(buf, pos);
	numGlyphs = Output16(buf, pos);
}

void head(const BYTE* buf, const UINT size, UINT16& indexToLocaFormat, INT& min_x, INT& max_x, INT& min_y, INT& max_y){
	UINT pos = 0;
	pos = 18;
	UINT16 unitsPerEm = Output16(buf, pos);
	pos = 36;
	INT16 xmin = Output16(buf, pos);	min_x = xmin;
	pos = 38;
	INT16 ymin = Output16(buf, pos);	min_y = ymin;
	pos = 40;
	INT16 xmax = Output16(buf, pos);	max_x = xmax;
	pos = 42;
	INT16 ymax = Output16(buf, pos);	max_y = ymax;
	pos = 50;
	indexToLocaFormat = Output16(buf, pos);
}

void loca(const BYTE* buf, const UINT size, UINT16 indexToLocaFormat, UINT numGlyphs, UINT*& glyphOffsets){
	UINT pos = 0;
	if(numGlyphs == 0){numGlyphs = 0xFFFF;}
	glyphOffsets = new UINT[numGlyphs+1];
	for(UINT i = 0 ; i<numGlyphs+1 ; ++i){
		if(indexToLocaFormat){glyphOffsets[i] = Output32(buf, pos);}
		else{glyphOffsets[i] = Output16(buf, pos);}
	}
}

void hhea(const BYTE* buf, const UINT size, UINT& numberOfHMetrics){
	UINT pos = 34;
	numberOfHMetrics = Output16(buf, pos);
}

bool hmtx(const BYTE* buf, const UINT size, UINT numHMat, UINT index, UINT16& advanceWidth){
	UINT pos = 0;
	UINT16 aw = 0;
	INT16 lsb = 0;

	if(numHMat > index){
		pos = index*4;
		aw  = Output16(buf, pos);
		lsb = Output16(buf, pos);
	}
	else{
		pos = (numHMat-1)*4;
		aw  = Output16(buf, pos);
		//pos = numHMat*4 + (index-numHMat)*2;
		pos = (numHMat+index)*2;
		lsb = Output16(buf, pos);
	}
	advanceWidth = aw;

	return true;
}

bool glyf_vtxSet(const BYTE* glyf_buf, const UINT glyf_size, UINT& pos, const UINT glyphIndex, GLYPH* glyph_array){
	INT16 xMin = Output16(glyf_buf, pos);
	INT16 yMin = Output16(glyf_buf, pos);
	INT16 xMax = Output16(glyf_buf, pos);
	INT16 yMax = Output16(glyf_buf, pos);

	glyph_array[glyphIndex].vtx[0] = {{(FLOAT)xMin, (FLOAT)yMax, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 0.0f}};
	glyph_array[glyphIndex].vtx[1] = {{(FLOAT)xMax, (FLOAT)yMax, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 0.0f}};
	glyph_array[glyphIndex].vtx[2] = {{(FLOAT)xMin, (FLOAT)yMin, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 1.0f}};
	glyph_array[glyphIndex].vtx[3] = {{(FLOAT)xMax, (FLOAT)yMin, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 1.0f}};
	glyph_array[glyphIndex].vtx_num = 4;

	glyph_array[glyphIndex].idx.index = new UINT[6];
	glyph_array[glyphIndex].idx.index[0] = 0;
	glyph_array[glyphIndex].idx.index[1] = 1;
	glyph_array[glyphIndex].idx.index[2] = 2;
	glyph_array[glyphIndex].idx.index[3] = 1;
	glyph_array[glyphIndex].idx.index[4] = 2;
	glyph_array[glyphIndex].idx.index[5] = 3;
	glyph_array[glyphIndex].idx.num = 6;

	return true;
}
bool glyf_Simple(const BYTE* glyf_buf, const UINT glyf_size, UINT& pos, const UINT cont_num, GLYF_DATA& glyph){
	//simple
	glyph.contours_num = cont_num;
	const UINT16* endOC = (UINT16*)&(glyf_buf[pos]);	pos += glyph.contours_num*2;
	UINT16 instLen = Output16(glyf_buf, pos);
	const BYTE* inst = &(glyf_buf[pos]);	pos += instLen;
	UINT max_point = Shift16(endOC[glyph.contours_num-1])+1;

	glyph.contours = new UINT[glyph.contours_num];

	for(UINT i = 0 ; i<glyph.contours_num ; ++i){
		glyph.contours[i] = Shift16(endOC[i]);
	}

	BYTE* flags = new BYTE[max_point]{};
	VECTOR2* tv = new VECTOR2[max_point]{};
	INT flag_repeat = 0;
	BYTE prev_flag = 0;
	UINT curOC = 0;
	INT curve_point = 0;

	for(UINT i = 0 ; i<max_point ; ++i){
		if(flag_repeat <= 0){
			flags[i] = Output8(glyf_buf, pos);
			if(flags[i] & 0x08){
				flag_repeat = Output8(glyf_buf, pos);
				prev_flag = flags[i];
			}
		}
		else{
			flags[i] = prev_flag;
			--flag_repeat;
		}

		//&0x1 / 1 - point / 0 - curve point
		if(!(flags[i]&0x01)){
			if(flags[i-1]&0x01){curve_point += 7;}
			--curve_point;
		}

		if(glyph.contours[curOC] == i){
			glyph.contours[curOC] += curve_point;
			++curOC;
		}
	}

	FLOAT prev_pos = 0.0f;
	//x
	for(UINT i = 0 ; i<max_point ; ++i){
		INT16 x = 0;
		if(flags[i] & 0x02){
			x = Output8(glyf_buf, pos);
			x = (flags[i] & 0x10)?x:-x;
		}
		else{
			if(flags[i] & 0x10){
				x = 0;
			}
			else{
				x = Output16(glyf_buf, pos);
			}
		}
		tv[i].x = x+prev_pos;
		prev_pos = x+prev_pos;
	}
	prev_pos = 0;
	//y
	for(UINT i = 0 ; i<max_point ; ++i){
		INT16 y = 0;
		if(flags[i] & 0x04){
			y = Output8(glyf_buf, pos);
			y = (flags[i] & 0x20)?y:-y;
		}
		else{
			if(flags[i] & 0x20){
				y = 0;
			}
			else{
				y = Output16(glyf_buf, pos);
			}
		}
		tv[i].y = y+prev_pos;
		prev_pos = y+prev_pos;
	}

	glyph.point = new VECTOR2[glyph.contours[glyph.contours_num-1]+1]{};

	curOC = 0;
	curve_point = 0;
	for(UINT i = 0, j = 0 ; i<max_point ; ++i){
		if(flags[i]&0x01){
			if(curve_point != 0){
				//bezier
				for(FLOAT b_t = 0.125f ; b_t < 1.0f ; b_t += 0.125f){
					switch(curve_point){
					case 1:		glyph.point[j++] = VECCAL::BezierSpline2(&(tv[i-curve_point-1]), b_t);				break;
					case 2:		glyph.point[j++] = VECCAL::BezierSpline3(&(tv[i-curve_point-1]), b_t);				break;
					default:	glyph.point[j++] = VECCAL::BezierSpline(&(tv[i-curve_point-1]), curve_point+2, b_t);	break;
					}
				}
				curve_point = 0;
			}
			glyph.point[j++] = tv[i];
		}
		else{
			++curve_point;
		}

		if(Shift16(endOC[curOC]) == i){
			if(curve_point > 0){
				//bezier
				for(FLOAT b_t = 0.125f ; b_t < 1.0f ; b_t += 0.125f){
					switch(curve_point){
					case 1:		glyph.point[j++] = VECCAL::BezierSpline2(&(tv[i-curve_point-1]), b_t);				break;
					case 2:		glyph.point[j++] = VECCAL::BezierSpline3(&(tv[i-curve_point-1]), b_t);				break;
					default:	glyph.point[j++] = VECCAL::BezierSpline(&(tv[i-curve_point-1]), curve_point+2, b_t);	break;
					}
				}
				curve_point = 0;
			}
			++curOC;
		}
	}

	delete[] tv;
	delete[] flags;

	return true;
}
bool glyf_Composite(const BYTE* glyf_buf, const UINT glyf_size,
					const BYTE* hmtx_buf, const UINT hmtx_size,
					UINT& pos, const UINT cont_num, 
					const UINT* glyphOffsets, const UINT numHMat, 
					GLYPH* glyph_array, GLYF_DATA& glyph){
	//composite
	/*
	Flags Bit Description 
	//0x0001 ARG_1_AND_2_ARE_WORDS
	0x0002 ARGS_ARE_XY_VALUES
	0x0004 ROUND_XY_TO_GRID
	//0x0008 WE_HAVE_A_SCALE
	-0x0010 RESERVED
	0x0020 MORE_COMPONENTS
	//0x0040 WE_HAVE_AN_X_AND_Y_SCALE
	//0x0080 WE_HAVE_A_TWO_BY_TWO
	0x0100 WE_HAVE_INSTRUCTIONS
	0x0200 USE_MY_METRICS
	-0x0400 OVERLAP_COMPOUND
	-0x0800 SCALED_COMPONENT_OFFSET
	0x1000 UNSCALED_COMPONENT_OFFSET
	-0x2000
	-0x4000
	-0x8000
	*/
	if(cont_num != -1){return false;}
	glyph.contours_num = 0;
	//std::cout<<"composite table is not set"<<std::endl;

	UINT16 flag = 0x0020;

	union b2i{
		BYTE b[4];
		UINT i;
	}conv;
	conv.i = 0;

	union b2v{
		BYTE b[8];
		VECTOR2 v;
	}convf;

	struct glypfSet{
		UINT idx;
		FLOAT matrix[6] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};	
		//scale_x = 0, scale_01 = 1, scale_10 = 2, scale_y = 3, 4 = arg1, 5 = arg2
	}set[256];

	bool complete = true;

	UINT set_num = 0;
	for( ; flag & 0x0020 ; ++set_num){
		flag = Output16(glyf_buf, pos);
		set[set_num].idx = Output16(glyf_buf, pos);
		//std::cout<<"flag : "<<flag<<std::endl;
		if(flag & 0xEC10){/*std::cout<<"reversed"<<std::endl;*/ complete =false; break;}
		//F2DOT == value/16384.0f
		if(flag & 0x0001){
			INT16 arg1 = Output16(glyf_buf, pos);	//(SHORT or FWord) argument1; 
			INT16 arg2 = Output16(glyf_buf, pos);	//(SHORT or FWord) argument2; 
			set[set_num].matrix[4] = arg1;
			set[set_num].matrix[5] = arg2;
		}
		else{
			INT16 arg1 = Output8(glyf_buf, pos);	//USHORT arg1and2; /* (arg1 << 8) | arg2 */ 
			INT16 arg2 = Output8(glyf_buf, pos);
			set[set_num].matrix[4] = arg1;
			set[set_num].matrix[5] = arg2;
		}
		if(flag & 0x0008){
			UINT16 s = Output16(glyf_buf, pos);	//F2Dot14  scale;    /* Format 2.14 */ 
			set[set_num].matrix[0] = (FLOAT)s/16384.0f;
			set[set_num].matrix[3] = (FLOAT)s/16384.0f;
		}
		else if(flag & 0x0040){
			UINT16 sx = Output16(glyf_buf, pos);	//F2Dot14  xscale;    /* Format 2.14 */ 
			UINT16 sy = Output16(glyf_buf, pos);	//F2Dot14  yscale;    /* Format 2.14 */ 
			set[set_num].matrix[0] = (FLOAT)sx/16384.0f;
			set[set_num].matrix[3] = (FLOAT)sy/16384.0f;
		}
		else if(flag & 0x0080){
			UINT16 sx = Output16(glyf_buf, pos);	//F2Dot14  xscale;    /* Format 2.14 */ 
			UINT16 s01 = Output16(glyf_buf, pos);	//F2Dot14  scale01;   /* Format 2.14 */ 
			UINT16 s10 = Output16(glyf_buf, pos);	//F2Dot14  scale10;   /* Format 2.14 */ 
			UINT16 sy = Output16(glyf_buf, pos);	//F2Dot14  yscale;    /* Format 2.14 */ 
			set[set_num].matrix[0] = (FLOAT)sx/16384.0f;
			set[set_num].matrix[1] = (FLOAT)s01/16384.0f;
			set[set_num].matrix[2] = (FLOAT)s10/16384.0f;
			set[set_num].matrix[3] = (FLOAT)sy/16384.0f;
		}

		bool b = false;
		b = SetGlyph(glyf_buf, glyf_size, hmtx_buf, hmtx_size, 
					 glyphOffsets, numHMat, set[set_num].idx, glyph_array);
		if(!b){/*std::cout<<"load glyf failed"<<std::endl;*/ complete = false; break;}
		conv.b[0] = glyph_array[set[set_num].idx].buf[0];
		conv.b[1] = glyph_array[set[set_num].idx].buf[1];
		conv.b[2] = glyph_array[set[set_num].idx].buf[2];
		conv.b[3] = glyph_array[set[set_num].idx].buf[3];
		glyph.contours_num += conv.i;
	}

	if(complete){
		glyph.contours = new UINT[glyph.contours_num];
		UINT cont_add = 0;
		for(UINT i = 0, cur_cont = 0 ; i<set_num ; ++i){
			conv.b[0] = glyph_array[set[i].idx].buf[0];
			conv.b[1] = glyph_array[set[i].idx].buf[1];
			conv.b[2] = glyph_array[set[i].idx].buf[2];
			conv.b[3] = glyph_array[set[i].idx].buf[3];
			UINT cur_set_cont_num = conv.i;
			GLYPH& cur_glyph = glyph_array[set[i].idx];
			for(UINT j = 0 ; j<cur_set_cont_num ; ++j, ++cur_cont){
				conv.b[0] = glyph_array[set[i].idx].buf[4+(j*4)+0];
				conv.b[1] = glyph_array[set[i].idx].buf[4+(j*4)+1];
				conv.b[2] = glyph_array[set[i].idx].buf[4+(j*4)+2];
				conv.b[3] = glyph_array[set[i].idx].buf[4+(j*4)+3];
				glyph.contours[cur_cont] = conv.i + cont_add;
			}
			conv.b[0] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+0];
			conv.b[1] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+1];
			conv.b[2] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+2];
			conv.b[3] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+3];
			cont_add += conv.i+1;
		}
		glyph.point = new VECTOR2[glyph.contours[glyph.contours_num-1]+1];
		for(UINT i = 0, cur_point = 0 ; i<set_num ; ++i){
			conv.b[0] = glyph_array[set[i].idx].buf[0];
			conv.b[1] = glyph_array[set[i].idx].buf[1];
			conv.b[2] = glyph_array[set[i].idx].buf[2];
			conv.b[3] = glyph_array[set[i].idx].buf[3];
			UINT cur_set_cont_num = conv.i;

			conv.b[0] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+0];
			conv.b[1] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+1];
			conv.b[2] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+2];
			conv.b[3] = glyph_array[set[i].idx].buf[4+((cur_set_cont_num-1)*4)+3];
			UINT cur_max_point = conv.i+1;
			for(UINT j = 0 ; j<cur_max_point ; ++j, ++cur_point){
				convf.b[0] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+0];
				convf.b[1] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+1];
				convf.b[2] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+2];
				convf.b[3] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+3];
				convf.b[4] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+4];
				convf.b[5] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+5];
				convf.b[6] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+6];
				convf.b[7] = glyph_array[set[i].idx].buf[4+(cur_set_cont_num*4)+(j*8)+7];
				VECTOR2 tp = convf.v;
				VECTOR2 trans = {0.0f, 0.0f};
				trans.x = (tp.x*set[i].matrix[0])+(tp.y*set[i].matrix[1])+set[i].matrix[4];
				trans.y = (tp.x*set[i].matrix[2])+(tp.y*set[i].matrix[3])+set[i].matrix[5];
				glyph.point[cur_point] = trans;
			}
		}
	}

	if(complete){return true;}
	else{return false;}
}

bool glyf(const BYTE* glyf_buf, const UINT glyf_size, 
		  const BYTE* hmtx_buf, const UINT hmtx_size,
		  const UINT* glyphOffsets, const UINT numHMat, 
		  UINT glyphIndex, GLYPH* glyph_array, 
		  GLYF_DATA& glyph){
	UINT pos = glyphOffsets[glyphIndex];

	INT16 cont_num = Output16(glyf_buf, pos);
	glyf_vtxSet(glyf_buf, glyf_size, pos, glyphIndex, glyph_array);

	if(cont_num > 0){return glyf_Simple(glyf_buf, glyf_size, pos, cont_num, glyph);}
	else{return glyf_Composite(glyf_buf, glyf_size, hmtx_buf, hmtx_size, pos, cont_num,
							   glyphOffsets, numHMat, glyph_array, glyph);}

	return false;
}

//format4 complate and other none complate
bool cmap(const BYTE* buf, const UINT size, UINT code, UINT& idx){
	UINT pos = 0;
	UINT version = Output16(buf, pos);
	UINT numTable = Output16(buf, pos);

	//std::cout<<"version : "<<version<<std::endl;
	//std::cout<<"Table number : "<<numTable<<std::endl;

	//Get Table Type
	{
		UINT t_pos = 0;
		for(UINT i = 0 ; i<numTable ; ++i){
			UINT16 pID = Output16(buf, pos);
			UINT16 eID = Output16(buf, pos);
			UINT offset = Output32(buf, pos);
			//std::cout<<"platform ID : "<<pID<<std::endl;
			//std::cout<<"encoding ID : "<<eID<<std::endl;
			//std::cout<<"offset : "<<offset<<std::endl;

			if(pID == 0 || (pID == 3 && (eID == 1 || eID == 10))){t_pos = offset;}
		}
		pos = t_pos;
	}

	UINT16 format = Output16(buf, pos);
	//std::cout<<"format : "<<format<<std::endl;

	switch(format){
	case 0: break;
	case 2: break;
	case 4:
	{
		UINT16 length = Output16(buf, pos);
		UINT16 language = Output16(buf, pos);
		UINT16 segCountX2 = Output16(buf, pos);
		UINT16 segCount = segCountX2/2;
		UINT16 searchRange = Output16(buf, pos);	searchRange /= 2;
		UINT16 entrySelector = Output16(buf, pos);
		UINT16 rangeShift = Output16(buf, pos);		rangeShift /= 2;
		const UINT16* endCode = (UINT16*)&(buf[pos]);	pos += segCountX2;
		UINT16 reservedPad = Output16(buf, pos);
		const UINT16* startCode = (UINT16*)&(buf[pos]);	pos += segCountX2;
		const UINT16* idDelta = (UINT16*)&(buf[pos]);	pos += segCountX2;
		const UINT16* idRangeOffset = (UINT16*)&(buf[pos]);	pos += segCountX2;

		UINT searchIndex = 0;
		if(code > 0xFFFF){return false;}
		if(code > Shift16(endCode[rangeShift])){searchIndex = rangeShift;}

		while(entrySelector){
			searchRange /= 2;
			if(code > Shift16(endCode[searchIndex+searchRange-1])){
				searchIndex += searchRange;
			}
			--entrySelector;
		}
		if(Shift16(startCode[searchIndex]) > code){return false;}

		UINT offset = Shift16(idRangeOffset[searchIndex]);
		if(offset == 0){
			idx = code + (INT16)Shift16(idDelta[searchIndex]);
			idx %= 0x10000;
		}
		else{
			idx = idRangeOffset[searchIndex+offset/2+(code-Shift16(startCode[searchIndex]))]; 
			idx = Shift16(idx);
		}
	}
	return true;
	case 6: break;
	case 8: break;
	case 10: break;
	case 12: break;
	case 13: break;
	case 14: break;
	}
	return false;
}

bool TrueTypeDecoder(const BYTE* buf, const UINT size, 
					 BYTE*& cmap_buf, UINT& cmap_size,
					 BYTE*& glyf_buf, UINT& glyf_size,
					 BYTE*& hmtx_buf, UINT& hmtx_size,
					 UINT*& glyphOffsets, UINT& numHMat,
					 GLYPH*& glyph, UINT& glyph_size,
					 INT& min_x, INT& max_x, INT& min_y, INT& max_y){
	UINT pos = 0;

	UINT version = Output32(buf, pos);

	if(version != 0x00010000){/*std::cout<<"version check failed"<<std::endl;*/ return false;}

	UINT numTable		= Output16(buf, pos);
	UINT serchRange		= Output16(buf, pos);
	UINT entrySelector	= Output16(buf, pos);
	UINT rangeShift		= Output16(buf, pos);

	//std::cout<<std::hex<<numTable<<std::endl;
	//std::cout<<std::hex<<serchRange<<std::endl;
	//std::cout<<std::hex<<entrySelector<<std::endl;
	//std::cout<<std::hex<<rangeShift<<std::endl;

	//if((numTable*16)-serchRange != rangeShift){/*std::cout<<"table num check failed"<<std::endl;*/}
	
	UINT16 indexToLocaFormat = 0;
	//numGlypf == glyf_size;
	
	for(UINT i = 0 ; i<numTable ; ++i){
		char tag[5] = {};
		UINT checksum = 0;
		UINT offset = 0;
		UINT length = 0;

		tag[0] = buf[pos++];
		tag[1] = buf[pos++];
		tag[2] = buf[pos++];
		tag[3] = buf[pos++];
		tag[4] = 0x00;

		checksum	= Output32(buf, pos);
		offset		= Output32(buf, pos);
		length		= Output32(buf, pos);

		UINT tagi = (tag[0]<<24) | (tag[1]<<16) | (tag[2]<<8) | tag[3];

#define s2i(s) ((s[0]<<24) | (s[1]<<16) | (s[2]<<8) | s[3])

		switch(tagi){
		case s2i("cmap"):	
			cmap_size = length;	
			cmap_buf = new BYTE[length];
			for(INT j = 0 ; j<length ; ++j){cmap_buf[j] = buf[offset+j];}
			TableChecksum(&(buf[offset]), length, checksum);
			break;
		case s2i("maxp"):	maxp(&(buf[offset]), size, glyph_size);	break;
		case s2i("head"):	head(&(buf[offset]), size, indexToLocaFormat, min_x, max_x, min_y, max_y);	break;
		case s2i("loca"):	loca(&(buf[offset]), size, indexToLocaFormat, glyph_size, glyphOffsets);	break;
		case s2i("hhea"):	hhea(&(buf[offset]), size, numHMat);	break;
		case s2i("hmtx"):	
			hmtx_size = length;	
			hmtx_buf = new BYTE[length];
			for(INT j = 0 ; j<length ; ++j){hmtx_buf[j] = buf[offset+j];}
			TableChecksum(&(buf[offset]), length, checksum);
			break;
		case s2i("glyf"):	
			glyf_size = length;	
			glyf_buf = new BYTE[length];
			for(INT j = 0 ; j<length ; ++j){glyf_buf[j] = buf[offset+j];}
			TableChecksum(&(buf[offset]), length, checksum);
			break;
		}
	}

#undef s2i

	++glyph_size;
	glyph = new GLYPH[glyph_size]{};

	return true;
}

bool SetGlyph(const BYTE* glyf_buf, const UINT glyf_size,
			  const BYTE* hmtx_buf, const UINT hmtx_size,
			  const UINT* glyphOffsets, const UINT numHMat,
			  const UINT index, GLYPH* glyph_array){
	if(glyph_array[index].buf != nullptr){return true;}

	GLYF_DATA glyf_data = {};
	UINT16 t_aw = 0;

	if(!glyf(glyf_buf, glyf_size, hmtx_buf, hmtx_size, 
			 glyphOffsets, numHMat, index, glyph_array, glyf_data)){return false;}
	hmtx(hmtx_buf, hmtx_size, numHMat, index, t_aw);
	glyph_array[index].advanceWidth = t_aw;

	UINT buf_size = 4 + (glyf_data.contours_num*4) + ((glyf_data.contours[glyf_data.contours_num-1]+1)*8);
	glyph_array[index].size = buf_size;
	glyph_array[index].buf = new BYTE[buf_size];
	{
		UINT pos = 0;
		union{
			BYTE* b;
			void* v;
		}v2b;

		v2b.v = &(glyf_data.contours_num);
		glyph_array[index].buf[pos+0] = v2b.b[0];
		glyph_array[index].buf[pos+1] = v2b.b[1];
		glyph_array[index].buf[pos+2] = v2b.b[2];
		glyph_array[index].buf[pos+3] = v2b.b[3];
		pos += 4;

		for(UINT i = 0 ; i<glyf_data.contours_num ; ++i){
			v2b.v = &(glyf_data.contours[i]);
			glyph_array[index].buf[pos+0] = v2b.b[0];
			glyph_array[index].buf[pos+1] = v2b.b[1];
			glyph_array[index].buf[pos+2] = v2b.b[2];
			glyph_array[index].buf[pos+3] = v2b.b[3];
			pos += 4;
		}
		for(UINT i = 0 ; i<glyf_data.contours[glyf_data.contours_num-1]+1 ; ++i){
			v2b.v = &(glyf_data.point[i].x);
			glyph_array[index].buf[pos+0] = v2b.b[0];
			glyph_array[index].buf[pos+1] = v2b.b[1];
			glyph_array[index].buf[pos+2] = v2b.b[2];
			glyph_array[index].buf[pos+3] = v2b.b[3];
			pos += 4;

			v2b.v = &(glyf_data.point[i].y);
			glyph_array[index].buf[pos+0] = v2b.b[0];
			glyph_array[index].buf[pos+1] = v2b.b[1];
			glyph_array[index].buf[pos+2] = v2b.b[2];
			glyph_array[index].buf[pos+3] = v2b.b[3];
			pos += 4;
		}
	}

	if(glyf_data.contours != nullptr){delete[] glyf_data.contours;}
	if(glyf_data.point != nullptr){delete[] glyf_data.point;}

	return true;
}

bool TrueTypeGlyph(const BYTE* cmap_buf, const UINT cmap_size, 
				   const BYTE* glyf_buf, const UINT glyf_size,
				   const BYTE* hmtx_buf, const UINT hmtx_size,
				   const UINT* glyphOffsets, const UINT numHMat,
				   UINT word, GLYPH*& glyph_array, GLYPH*& glyph_output){
	UINT index = 0;
	if(!cmap(cmap_buf, cmap_size, word, index)){return false;}
	
	if(!SetGlyph(glyf_buf, glyf_size, hmtx_buf, hmtx_size, 
				 glyphOffsets, numHMat, index, glyph_array)){return false;}

	glyph_output = &(glyph_array[index]);
	
	return true;
}

#undef Output8
#undef Output16
#undef Output24
#undef Output32
#undef Shift16
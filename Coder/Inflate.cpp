#include "Inflate.h"
#include "Adler32.h"

//UINT len[29] =   {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
//UINT extra[29] = {0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
//UINT dist[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
//UINT extra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

struct HuffmanTree{
	INT len;
	INT code;
};

UINT GetLength(INT num){
	if(num < 257){return num;}
	num -= 257;
	UINT len[29] =   {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
	if(0 <= num && num < 29){return len[num];}
	else{return 0;}
}

UINT GetDistance(INT num){
	UINT dist[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
	if(0 <= num && num < 30){return dist[num];}
	else{return 0;}
}

UINT GetLengthExtra(INT num){
	num -= 257;
	UINT extra[29] = {0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
	if(0 <= num && num < 29){return extra[num];}
	else{return 0;}
}

UINT GetDistanceExtra(INT num){
	UINT extra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
	if(0 <= num && num < 30){return extra[num];}
	else{return 0;}
}

UINT GetCodeLengthMap(INT num){
	UINT map[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
	return map[num];
}

#define next_bit {bit = cbuf[++cpos]; bit_pos = 0;}
#define bit_copy(c, a) {c = 0; for(INT b = 0 ; b<a ; ++b){c |= (bit&0x01)<<b; bit>>=1; ++bit_pos; if(bit_pos==8){next_bit;}}}
#define bit_shift(a) {for(INT b = 0 ; b<a ; ++b){bit>>=1; ++bit_pos; if(bit_pos==8){next_bit;}}}
#define bit_set bit, bit_pos, cbuf, cpos
#define bit_get BYTE& bit, UINT& bit_pos, BYTE* cbuf, UINT& cpos

INT GetData(HuffmanTree* tree, INT max, bit_get){
	INT temp = 0;
	temp<<=1; temp |= bit&0x01; bit>>=1; bit_pos+=1; if(bit_pos==8){next_bit;}
	INT temp_count = 1;
	while((tree[temp].len == 0 || tree[temp].len != temp_count) && temp_count<max){
		++temp_count;
		temp<<=1; temp |= bit&0x01; bit>>=1; bit_pos+=1; if(bit_pos==8){next_bit;}
	}
	if(tree[temp].len != temp_count){return -1;}
	else{
		return temp;
	}
}

void CreateCodeTree(HuffmanTree*& tree, INT size, INT& max, bit_get){
	max = 0;

	INT len[19] = {};
	for(INT j = 0 ; j<size ; ++j){
		INT temp = 0;
		bit_copy(temp, 3);
		len[GetCodeLengthMap(j)] = temp;
	}

	INT bl_count[16] = {};
	INT next_code[16] = {};

	for(INT j = 0 ; j<19 ; ++j){
		if(len[j] != 0){
			++bl_count[len[j]];
			max = (max<len[j])?len[j]:max;
		}
	}

	bl_count[0] = 0;

	{
		INT code = 0;

		for(INT j = 1 ; j<max+1 ; ++j){
			code = (code+bl_count[j-1])<<1;
			next_code[j] = code;
		}
	}

	tree = new HuffmanTree[1<<max];
	for(INT j = 0 ; j<(1<<max) ; ++j){tree[j] = {};}

	for(INT j = 0 ; j<19 ; ++j){
		INT l = len[j];
		if(l != 0){
			tree[next_code[l]++] = {l, j};
		}
	}
}

void CreateDynamicTree(HuffmanTree*& tree, INT size, INT& max, HuffmanTree* ctree, INT bl_max, bit_get){
	INT len[320] = {};
	INT bl_count[16] = {};
	INT next_code[16] = {};

	for(INT j = 0 ; j<size ; ++j){
		INT temp = GetData(ctree, bl_max, bit_set);
		if(temp == -1){return;}

		if(ctree[temp].code < 16){
			len[j] = ctree[temp].code;
			++bl_count[len[j]];
		}
		else if(ctree[temp].code == 16){
			INT extra_bit = 0;	//3-6
			bit_copy(extra_bit, 2);
			extra_bit += 3;
			for(INT k = 0 ; k<extra_bit ; ++k, ++j){len[j] = len[j-1]; ++bl_count[len[j]];}
			--j;
		}
		else if(ctree[temp].code == 17){
			INT extra_bit = 0;	//3-10
			bit_copy(extra_bit, 3);
			extra_bit += 3;
			for(INT k = 0 ; k<extra_bit ; ++k, ++j){len[j] = 0; ++bl_count[len[j]];}
			--j;
		}
		else if(ctree[temp].code == 18){
			INT extra_bit = 0;	//11-138
			bit_copy(extra_bit, 7);
			extra_bit += 11;
			for(INT k = 0 ; k<extra_bit ; ++k, ++j){len[j] = 0; ++bl_count[len[j]];}
			--j;
		}

		if(len[j] != 0){
			max = (max<len[j])?len[j]:max;
		}
	}
	bl_count[0] = 0;

	{
		INT code = 0;

		for(INT j = 1 ; j<max+1 ; ++j){
			code = (code+bl_count[j-1])<<1;
			next_code[j] = code;
		}
	}

	tree = new HuffmanTree[1<<max];
	for(INT j = 0 ; j<(1<<max) ; ++j){tree[j] = {};}

	for(INT j = 0 ; j<size ; ++j){
		INT l = len[j];
		if(l != 0){
			tree[next_code[l]++] = {l, j};
		}
	}
}

void DynamicHuffman(bit_get, BYTE*& ucbuf, UINT& ucsize, UINT& ucpos){
	INT HLIT = 0;
	INT HDIST = 0;
	INT HCLEN = 0;
	bit_copy(HLIT, 5);
	bit_copy(HDIST, 5);
	bit_copy(HCLEN, 4);
	HLIT += 257;
	HDIST += 1;
	HCLEN += 4;
	INT HTLEN = HLIT+HDIST;

	HuffmanTree* ctree = nullptr;
	INT bl_max = 0;
	CreateCodeTree(ctree, HCLEN, bl_max, bit_set);

	HuffmanTree* hlit_tree = nullptr;
	INT l_max_L = 0;
	CreateDynamicTree(hlit_tree, HLIT, l_max_L, ctree, bl_max, bit_set);

	HuffmanTree* hdist_tree = nullptr;
	INT l_max_D = 0;
	CreateDynamicTree(hdist_tree, HDIST, l_max_D, ctree, bl_max, bit_set);

	//data uncomp
	while(true){
		INT temp = GetData(hlit_tree, l_max_L, bit_set);
		if(temp == -1){return;}

		if(hlit_tree[temp].code < 256){ucbuf[ucpos++] = hlit_tree[temp].code;}
		else if(hlit_tree[temp].code == 256){break;}
		else if(hlit_tree[temp].code > 256){
			INT len = 0;
			bit_copy(len, GetLengthExtra(hlit_tree[temp].code));
			len += GetLength(hlit_tree[temp].code);

			INT temp2 = GetData(hdist_tree, l_max_D, bit_set);
			if(temp2 == -1){return;}

			INT dist = 0;
			bit_copy(dist, GetDistanceExtra(hdist_tree[temp2].code));
			dist += GetDistance(hdist_tree[temp2].code);

			//lz77 output
			{
				INT lzpos = ucpos-dist;
				for(INT j = 0 ; j<len ; ++j){
					ucbuf[ucpos++] = ucbuf[lzpos++];
				}
			}
		}
	}

	delete[] ctree;
	delete[] hlit_tree;
	delete[] hdist_tree;
}

void FixedHuffman(bit_get, BYTE*& ucbuf, UINT& ucsize, UINT& ucpos){
	//Lit :   0-143 / Bit = 8 / 0x30-0xBF
	//Lit : 144-255 / Bit = 9 / 0x190-0x1FF
	//Lit : 256-279 / Bit = 7 / 0x00-0x17
	//Lit : 280-287 / Bit = 8 / 0xC0-0xC7
	//distance 5bit

	while(true){
		INT lit_bit = 0;
		INT lit = 0;
		for(INT j = 0 ; j<7 ; ++j){
			lit_bit <<= 1; lit_bit |= bit&0x01; bit>>=1; ++bit_pos; if(bit_pos == 8){next_bit;}
		}
		if(0x00 <= lit_bit && lit_bit <= 0x17){lit = 256+(lit_bit-0x00);}
		else{
			lit_bit <<= 1; lit_bit |= bit&0x01; bit>>=1; ++bit_pos; if(bit_pos == 8){next_bit;}
			if(0x30 <= lit_bit && lit_bit <= 0xBF){lit = 0+(lit_bit-0x30);}
			else if(0xC0 <= lit_bit && lit_bit <= 0xC7){lit = 280+(lit_bit-0xC0);}
			else{
				lit_bit <<= 1; lit_bit |= bit&0x01; bit>>=1; ++bit_pos; if(bit_pos == 8){next_bit;}
				if(0x190 <= lit_bit && lit_bit <= 0x1FF){lit = 144+(lit_bit-0x190);}
				else{return;}
			}
		}

		if(lit < 256){ucbuf[ucpos++] = lit;}
		else if(lit == 256){break;}
		else if(lit > 256){
			INT len = 0;
			bit_copy(len, GetLengthExtra(lit));
			len += GetLength(lit);

			INT dist_bit = 0;
			for(INT j = 0 ; j<5 ; ++j){
				dist_bit <<= 1; dist_bit |= bit&0x01; bit >>= 1; ++bit_pos; if(bit_pos == 8){next_bit;}
			}
			if(dist_bit == 30 || dist_bit == 31){return;}
			INT dist = 0;
			bit_copy(dist, GetDistanceExtra(dist_bit));
			dist += GetDistance(dist_bit);
			//lz77 output
			{
				INT lzpos = ucpos-dist;
				for(INT j = 0 ; j<len ; ++j){
					ucbuf[ucpos++] = ucbuf[lzpos++];
				}
			}
		}
	}
}

bool Inflate(BYTE*& cbuf, UINT& csize, UINT& cpos, BYTE*& ucbuf, UINT& ucsize, UINT& ucpos){
	if(ucsize == 0 || ucbuf == nullptr || csize == 0 || cbuf == nullptr){return false;}
	
	BYTE bit = cbuf[cpos];
	UINT bit_pos = 0;

	while(true){
		bool last = false;
		bit_copy(last, 1);

		UINT type = 0;
		bit_copy(type, 2);

		switch(type){
		case 0x00:
		{
			if(bit_pos != 0) next_bit;
			UINT len = cbuf[cpos++];
			len |= cbuf[cpos++]<<8;
			UINT nlen = cbuf[cpos++];
			nlen |= cbuf[cpos++]<<8;
			if(len != (nlen^0xFFFF)){return false;}
			for(UINT j = 0 ; j<len ; ++j){ucbuf[ucpos++] = cbuf[cpos++];}
			bit = cbuf[cpos];
		}
		break;
		case 0x01:
			FixedHuffman(bit_set, ucbuf, ucsize, ucpos);
			break;
		case 0x02:
			DynamicHuffman(bit_set, ucbuf, ucsize, ucpos);
			break;
		case 0x03:
			return false;
			break;
		}

		if(last){break;}
	}

	if(bit_pos != 0){++cpos;}

	return true;
}

bool DataUncompress(BYTE*& cbuf, UINT& csize, UINT& cpos, BYTE*& ucbuf, UINT& ucsize, UINT& ucpos){
	if(ucsize == 0 || ucbuf == nullptr || csize == 0 || cbuf == nullptr){return false;}

	union CMFnFLG{
		BYTE b[2];
		INT16 s;
	};

	CMFnFLG cnf = {};

	cnf.b[0] = cbuf[cpos++]; 
	cnf.b[1] = cbuf[cpos++];
	if((cnf.s % 0x1F) == 0){return false;}
	if((cnf.b[1] & 0x20) != 0){cpos+=4;}

	BYTE bit = cbuf[cpos];
	UINT bit_pos = 0;

	while(true){
		bool last = false;
		bit_copy(last, 1);

		UINT type = 0;
		bit_copy(type, 2);

		switch(type){
		case 0x00:
		{
			next_bit;
			UINT len = cbuf[cpos++];
			len |= cbuf[cpos++]<<8;
			UINT nlen = cbuf[cpos++];
			nlen |= cbuf[cpos++]<<8;
			if(len != (nlen^0xFFFF)){return false;}
			for(UINT j = 0 ; j<len ; ++j){ucbuf[ucpos++] = cbuf[cpos++];}
			bit = cbuf[cpos];
		}
		break;
		case 0x01:
			FixedHuffman(bit_set, ucbuf, ucsize, ucpos);
			break;
		case 0x02:
			DynamicHuffman(bit_set, ucbuf, ucsize, ucpos);
			break;
		case 0x03:
			return false;
			break;
		}

		if(last){break;}
	}

	if(bit_pos != 0){++cpos;}

	INT adler32 = cbuf[cpos++]<<24;
	adler32 |= cbuf[cpos++]<<16;
	adler32 |= cbuf[cpos++]<<8;
	adler32 |= cbuf[cpos++];

	if(!Adler32(ucbuf, ucpos, adler32)){return false;}

	return true;
}

#undef bit_set
#undef bit_get
#undef bit_shift
#undef bit_copy
#undef next_bit

#pragma once

#include "..\\Common\\variable.h"

//Literal 0-255 / 256 = end of block

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
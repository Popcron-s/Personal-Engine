#include "Adler32.h"

bool Adler32(BYTE* buf, UINT len, UINT checksum){
	UINT A = 1;
	UINT B = 0;

	for(INT i = 0 ; i<len ; ++i){
		A += buf[i];
		A %= 65521;
		B += A;
		B %= 65521;
	}

	UINT result = A | (B<<16);

	if(result == checksum){return true;}
	else{return false;}
}
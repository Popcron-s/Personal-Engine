#include "ZIPDecoder.h"
#include "Inflate.h"

bool ZIPDecoder(BYTE* buf, UINT size, UINT*& LH_offset, UINT& LH_size){
	INT idx = 0;
	for( ; idx<size ; ++idx){
		if(buf[idx] == 0x50 && buf[idx+1] == 0x4B && buf[idx+2] == 0x05 && buf[idx+3] == 0x06){break;}
	}

	union{
		UINT i;
		BYTE b[4];
	}b2i;

	b2i.b[0] = buf[idx+8];
	b2i.b[1] = buf[idx+9];
	b2i.b[2] = 0x00;
	b2i.b[3] = 0x00;

	LH_size = b2i.i;
	LH_offset = new UINT[LH_size];

	INT LH_idx = 0;
	for(idx = 0 ; idx<size ; ++idx){
		if(buf[idx] == 0x50 && buf[idx+1] == 0x4B && buf[idx+2] == 0x01 && buf[idx+3] == 0x02){
			b2i.b[0] = buf[idx+42];
			b2i.b[1] = buf[idx+43];
			b2i.b[2] = buf[idx+44];
			b2i.b[3] = buf[idx+45];
			LH_offset[LH_idx++] = b2i.i;
		}
		if(buf[idx] == 0x50 && buf[idx+1] == 0x4B && buf[idx+2] == 0x05 && buf[idx+3] == 0x06){break;}
	}

	return true;
}

bool Decompression(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name, BYTE*& decomp_buf, UINT& decomp_size){
	if(decomp_buf != nullptr){
		delete[] decomp_buf;
		decomp_buf = nullptr;
		decomp_size = 0;
	}
	
	if(zip_buf[offset  ] != 0x50 || zip_buf[offset+1] != 0x4B || 
	   zip_buf[offset+2] != 0x03 || zip_buf[offset+3] != 0x04){return false;}

	union{
		UINT i;
		BYTE b[4];
	}b2i;

	INT compression_method = 0;
	INT file_name_length = 0;
	UINT CRC = 0;
	UINT comp_size;

	INT extra_field_length = 0;

	b2i.b[0] = zip_buf[offset+8];
	b2i.b[1] = zip_buf[offset+9];
	b2i.b[2] = 0x00;
	b2i.b[3] = 0x00;
	compression_method = b2i.i;

	b2i.b[0] = zip_buf[offset+14];
	b2i.b[1] = zip_buf[offset+15];
	b2i.b[2] = zip_buf[offset+16];
	b2i.b[3] = zip_buf[offset+17];
	CRC = b2i.i;

	b2i.b[0] = zip_buf[offset+18];
	b2i.b[1] = zip_buf[offset+19];
	b2i.b[2] = zip_buf[offset+20];
	b2i.b[3] = zip_buf[offset+21];
	comp_size = b2i.i;

	b2i.b[0] = zip_buf[offset+22];
	b2i.b[1] = zip_buf[offset+23];
	b2i.b[2] = zip_buf[offset+24];
	b2i.b[3] = zip_buf[offset+25];
	decomp_size = b2i.i;

	b2i.b[0] = zip_buf[offset+26];
	b2i.b[1] = zip_buf[offset+27];
	b2i.b[2] = 0x00;
	b2i.b[3] = 0x00;
	file_name_length = b2i.i;

	b2i.b[0] = zip_buf[offset+28];
	b2i.b[1] = zip_buf[offset+29];
	b2i.b[2] = 0x00;
	b2i.b[3] = 0x00;
	extra_field_length = b2i.i;

	for(INT i = 0 ; i<file_name_length ; ++i){
		file_name[i] = zip_buf[offset+30+i];
	}
	file_name[file_name_length] = 0x00;

	if(compression_method == 0x08){
		decomp_buf = new BYTE[decomp_size];
		UINT cpos = offset+30+file_name_length+extra_field_length;
		UINT dcpos = 0;
		Inflate(zip_buf, comp_size, cpos, decomp_buf, decomp_size, dcpos);
		cpos = 0;
		dcpos = 0;
	}
	else if(compression_method == 0x00){
		decomp_buf = new BYTE[decomp_size];
		for(INT i = 0 ; i<decomp_size ; ++i){
			decomp_buf[i] = zip_buf[offset+4+i];
		}
	}
	else{return false;}

	return true;
}

bool GetFilename(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name){
	if(zip_buf[offset  ] != 0x50 || zip_buf[offset+1] != 0x4B || 
	   zip_buf[offset+2] != 0x03 || zip_buf[offset+3] != 0x04){return false;}

	INT file_name_length = zip_buf[offset+26] | (zip_buf[offset+27]<<8);

	for(INT i = 0 ; i<file_name_length ; ++i){
		file_name[i] = zip_buf[offset+30+i];
	}
	file_name[file_name_length] = 0x00;

	return true;
}
#include "Vorbis.h"
#include "VorbisHeader.h"
#include "VorbisPacket.h"

#include "..\\Common\\Sort.h"

#define setter buf, size, pos, bit, bit_pos

bool Init_decode_codebook_slow(vorbis_codebook& cb){
	UINT count = 0;
	for(INT i = 0, marker[33] = {} ; i<cb.code_size ; ++i){
		UINT len = cb.code[i].length;
		cb.code[i].word = 0;
		if(len > 0){
			UINT entry = marker[len];
			cb.code[i].word = entry;

			for(INT j = len ; j>0 ; --j){
				if(marker[j]&1){
					if(j == 1){++marker[1];}
					else{marker[j] = marker[j-1]<<1;}
					break;
				}
				++marker[j];
			}

			for(INT j = len+1 ; j<33 ; ++j){
				if((marker[j]>>1) == entry){
					entry = marker[j];
					marker[j] = marker[j-1]<<1;
				}
				else{break;}
			}
		}
	}
	return true;
}

bool Init_decode_codebook(vorbis_codebook& cb){
	UINT count = 0;
	for(INT i = 0, marker[33] = {} ; i<cb.code_size ; ++i){
		UINT len = cb.code[i].length;
		cb.code[i].entry = i;
		cb.code[i].word = 0;
		if(len > 0){
			UINT entry = marker[len];
			cb.code[i].word = entry;

			for(INT j = len ; j>0 ; --j){
				if(marker[j]&1){
					if(j == 1){++marker[1];}
					else{marker[j] = marker[j-1]<<1;}
					break;
				}
				++marker[j];
			}

			for(INT j = len+1 ; j<33 ; ++j){
				if((marker[j]>>1) == entry){
					entry = marker[j];
					marker[j] = marker[j-1]<<1;
				}
				else{break;}
			}
		}
	}

	bool (*func)(vorbis_codebook::block a, vorbis_codebook::block b) = 
		[](vorbis_codebook::block a, vorbis_codebook::block b)->bool{
			return a.length<b.length;
		};

	Sort::GetSingleton()->Quick(cb.code, cb.code+cb.code_size, func);

	return true;
}

bool VorbisDecode_iHeader(BYTE* buf, UINT size, Identification_Header& iHeader){
	union{
		BYTE* b;
		UINT* i;
	}b2i;

	UINT pos = 0;

	BYTE packet = buf[pos++];
	pos += 6;	//if(buf[pos+0 ~ pos+5] == vorbis)

	if(packet == 1){	//identification_header
		b2i.b = &(buf[pos]);	pos += 4;
		iHeader.channel = buf[pos++];
		b2i.b = &(buf[pos]);	pos += 4;
		iHeader.samplerate = *(b2i.i);
		pos += 12;
		UINT bsize = buf[pos++];
		UINT bsize0 = bsize & 0xF;
		UINT bsize1 = (bsize & 0xF0)>>4;
		iHeader.blocksize_0 = 1<<bsize0;
		iHeader.blocksize_1 = 1<<bsize1;
		BYTE framing_flag = buf[pos++] & 0x1;
		if(framing_flag == 0 ||
		   iHeader.channel == 0 || iHeader.samplerate == 0 ||
		   (6 > bsize0 || bsize0 > 13) || (6 > bsize1 || bsize1 > 13) ||
		   bsize0 >= bsize1){return false;}
		else{return true;}
	}
	else{return false;}
}
bool VorbisDecode_sHeader(BYTE* buf, UINT size, Identification_Header& iHeader, Setup_Header& sHeader){
	union{
		BYTE* b;
		UINT* i;
	}b2i;

	BYTE bit = buf[0];
	UINT bit_pos = 0;
	UINT pos = 0;

	while(true){
		BYTE packet = buf[pos++];
		pos += 6;	//if(buf[pos+0 ~ pos+5] == vorbis)

		if(packet == 3){	//comment header
			b2i.b = &(buf[pos]);	pos += 4;
			UINT t = *(b2i.i);
			pos += t;
			b2i.b = &(buf[pos]);	pos += 4;
			t = *(b2i.i);
			for(UINT i = 0 ; i<t ; ++i){
				b2i.b = &(buf[pos]);	pos += 4;
				UINT t2 = *(b2i.i);
				pos += t2;
			}
			BYTE framing_flag = buf[pos++] & 0x1;
			if(framing_flag == 0){return false;}
		}
		else if(packet == 5){	//setup header
			bit = buf[pos];

			if(!VorbisCodebookHeaderDecoder(setter, sHeader))			{return false;}
			if(!VorbisTimeDomainTransformHeaderDecoder(setter))			{return false;}
			if(!VorbisFloorHeaderDecoder(setter, sHeader))				{return false;}
			if(!VorbisResidueHeaderDecoder(setter, sHeader))			{return false;}
			if(!VorbisMappingHeaderDecoder(setter, iHeader, sHeader))	{return false;}
			if(!VorbisModeHeaderDecoder(setter, sHeader))				{return false;}

			for(INT i = 0 ; i<sHeader.cb_size ; ++i){Init_decode_codebook(sHeader.cb[i]);}

			return true;
		}
		else{return false;}
	}
}
void VorbisRelease_sHeader(Setup_Header& sHeader){ReleaseHeader(sHeader);}

bool VorbisDecode_Packet(BYTE* buf, UINT size, Identification_Header& iHeader, Setup_Header& sHeader, FLOAT* cos_map_0, FLOAT* cos_map_1, vorbis_window& vWindow){
	return VorbisPacketDecode(buf, size, iHeader, sHeader, cos_map_0, cos_map_1, vWindow);
}

#undef setter
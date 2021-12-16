#include "WAVDecoder.h"

bool WAVDecoder(BYTE* buf, UINT size, SOUND& sound_data){
	if(buf == nullptr){return false;}
	if(buf[0] != 'R' || buf[1] != 'I' || buf[2] != 'F' || buf[3] != 'F' || 
		buf[8] != 'W' || buf[9] != 'A' || buf[10] != 'V' || buf[11] != 'E' ){
		return false;
	}

	UINT pos = 12;

#define s2i(s) (s[0] << 24 | (s[1] << 16) | (s[2] << 8) | (s[3]))
	while(pos<size){
		UINT string = buf[pos+0] << 24 | (buf[pos+1] << 16) | (buf[pos+2] << 8) | buf[pos+3];
		switch(string){
		case s2i("fmt "):
		{
			pos += 4;
			UINT tsize = buf[pos+3] << 24 | (buf[pos+2] << 16) | (buf[pos+1] << 8) | buf[pos+0];
			pos += 4;
			sound_data.wFormatTag		= buf[pos+1]<<8 | buf[pos+0]; pos += 2;    
			sound_data.nChannels		= buf[pos+1]<<8 | buf[pos+0]; pos += 2;     
			sound_data.nSamplesPerSec	= buf[pos+3] << 24 | (buf[pos+2] << 16) | (buf[pos+1] << 8) | buf[pos+0]; pos += 4;
			sound_data.nAvgBytesPerSec	= buf[pos+3] << 24 | (buf[pos+2] << 16) | (buf[pos+1] << 8) | buf[pos+0]; pos += 4;
			sound_data.nBlockAlign		= buf[pos+1]<<8 | buf[pos+0]; pos += 2;   
			sound_data.wBitsPerSample	= buf[pos+1]<<8 | buf[pos+0]; pos += 2;
			sound_data.cbSize			= 0;
			/*pos += 4;
			sound_data.chnbit = buf[pos+1] << 24 | (buf[pos+0] << 16) | (buf[pos+3] << 8) | buf[pos+2];
			pos += 4;
			sound_data.samplerate = buf[pos+3] << 24 | (buf[pos+2] << 16) | (buf[pos+1] << 8) | buf[pos+0];
			pos += 4;*/
		}
		break;
		case s2i("data"):
		{
			pos += 4;
			UINT tsize = buf[pos+3] << 24 | (buf[pos+2] << 16) | (buf[pos+1] << 8) | buf[pos+0];
			pos += 4;
			sound_data.size = tsize;
			sound_data.buf = new BYTE[tsize];
			for(UINT i = 0 ; i<tsize ; ++i){
				sound_data.buf[i] = buf[++pos];
			}
		}
		break;
		default:
			++pos;
			break;
		}
	}
#undef s2i

	return true;
}
#include "OGGDecoder.h"
#include "Vorbis.h"

bool OGGDecoder(BYTE* buf, UINT size, SOUND& sound_data){
	if(buf == nullptr){return false;}

	char signature[4] = "";
	UINT checksum = 0;
	UINT page_seq = 0;
	UINT page_seg = 0;

	bool b = true;

	UINT offset = 0;
	UINT cur_seq = 0;

	Identification_Header iHeader = {};
	Setup_Header sHeader = {};
	vorbis_window prev_vWin = {false, false, false, 0, nullptr};
	vorbis_window cur_vWin = {false, false, false, 0, nullptr};

	FLOAT* b0_Win = nullptr;
	FLOAT* b1_Win = nullptr;

	UINT b0_c = 0;
	UINT b1_c = 0;

	INT64 granule = 0;

	FLOAT* cos_map_0 = nullptr;
	FLOAT* cos_map_1 = nullptr;

	//iHeader
	if(b){
		signature[0] = buf[offset+0];
		signature[1] = buf[offset+1];
		signature[2] = buf[offset+2];
		signature[3] = buf[offset+3];

		checksum = *((UINT*)&(buf[offset+22]));
		page_seq = *((UINT*)&(buf[offset+18]));
		page_seg = buf[offset+26];

		if(signature[0] != 'O' || signature[1] != 'g' || 
		   signature[2] != 'g' || signature[3] != 'S' ||
		   page_seq != cur_seq++){return false;}

		granule = *((INT64*)&(buf[offset+6]));

		offset += 27;
		UINT seg_offset = offset;
		offset += page_seg;
		UINT page_offset = offset;

		UINT size = 0;
		for(UINT j = 0 ; j<page_seg ; ++j){
			size += buf[seg_offset+j];
		}
		b = VorbisDecode_iHeader(&(buf[page_offset]), size, iHeader);
		offset += size;
	}
	
	//sHeader
	if(b){
		signature[0] = buf[offset+0];
		signature[1] = buf[offset+1];
		signature[2] = buf[offset+2];
		signature[3] = buf[offset+3];

		checksum = *((UINT*)&(buf[offset+22]));
		page_seq = *((UINT*)&(buf[offset+18]));
		page_seg = buf[offset+26];

		if(signature[0] != 'O' || signature[1] != 'g' || 
		   signature[2] != 'g' || signature[3] != 'S' ||
		   page_seq != cur_seq++){return false;}

		granule = *((INT64*)&(buf[offset+6]));

		offset += 27;
		UINT seg_offset = offset;
		offset += page_seg;
		UINT page_offset = offset;

		UINT size = 0;
		for(UINT j = 0 ; j<page_seg ; ++j){
			size += buf[seg_offset+j];
		}
		b = VorbisDecode_sHeader(&(buf[page_offset]), size, iHeader, sHeader);
		offset += size;
	}
	if(!b){VorbisRelease_sHeader(sHeader);}
	
	//packet
	for(UINT i = offset ; i<size ; ){
		signature[0] = buf[i+0];
		signature[1] = buf[i+1];
		signature[2] = buf[i+2];
		signature[3] = buf[i+3];

		checksum = *((UINT*)&(buf[i+22]));
		page_seq = *((UINT*)&(buf[i+18]));
		page_seg = buf[i+26];

		if(signature[0] != 'O' || signature[1] != 'g' || 
		   signature[2] != 'g' || signature[3] != 'S' ||
		   page_seq != cur_seq++){b = false; break;}

		granule = *((INT64*)&(buf[i+6]));

		i += 27;
		UINT seg_offset = i;
		i += page_seg;

		for(UINT j = 0 ; j<page_seg ; ++j){
			i += buf[seg_offset+j];
		}
	}

	if(granule == 0){b = false;}

	if(b){
		sound_data.wFormatTag = 1;
		sound_data.nChannels = iHeader.channel;
		sound_data.nSamplesPerSec = iHeader.samplerate;
		sound_data.nAvgBytesPerSec = 2*iHeader.channel*iHeader.samplerate;
		sound_data.nBlockAlign = 2*iHeader.channel;
		sound_data.wBitsPerSample = 16;
		sound_data.cbSize = 0;
		sound_data.samplerate = 0;	/*unused*/

		sound_data.size = granule*2*iHeader.channel;
		sound_data.buf = new BYTE[sound_data.size];

		//Create Window
		b0_Win = new FLOAT[iHeader.blocksize_0];
		b1_Win = new FLOAT[iHeader.blocksize_1];

		b0_c = iHeader.blocksize_0>>1;
		b1_c = iHeader.blocksize_1>>1;

		FLOAT half_pi = 3.141592f*0.5f;

		for(UINT i = 0 ; i<b0_c ; ++i){
			FLOAT t = sin((i+0.5f)/b0_c*half_pi);
			b0_Win[i] = sin(half_pi*t*t);
		}
		for(UINT i = b0_c ; i<iHeader.blocksize_0 ; ++i){
			FLOAT t = sin((((i-b0_c))+0.5f)/b0_c*half_pi+half_pi);
			b0_Win[i] = sin(half_pi*t*t);
		}
		for(UINT i = 0 ; i<b1_c ; ++i){
			FLOAT t = sin((i+0.5f)/b1_c*half_pi);
			b1_Win[i] = sin(half_pi*t*t);
		}
		for(UINT i = b1_c ; i<iHeader.blocksize_1 ; ++i){
			FLOAT t = sin((((i-b1_c))+0.5f)/b1_c*half_pi+half_pi);
			b1_Win[i] = sin(half_pi*t*t);
		}

		//fast imdct cosine map
		//Cm_n = 1.0f/(2.0f*cos(pi*((2.0f*n)+1.0f+N2)/(2.0f*m)))
		//m = 2<=m<=N
		//n = 0<=n<m
		UINT t_size = iHeader.blocksize_0<<1;
		UINT N2 = iHeader.blocksize_0>>1;
		cos_map_0 = new FLOAT[t_size];
		cos_map_0[0] = 0.0f;
		cos_map_0[1] = 1.0f;

		for(UINT i = 2 ; i<=iHeader.blocksize_0 ; i<<=1){
			for(UINT j = 0 ; j<i ; ++j){
				cos_map_0[i+j] = 1.0f/(2.0f*cos(3.141592f*((2.0f*j)+1.0f+N2)/(2.0f*i)));
			}
		}

		t_size = iHeader.blocksize_1<<1;
		N2 = iHeader.blocksize_1>>1;
		cos_map_1 = new FLOAT[t_size];
		cos_map_1[0] = 0.0f;
		cos_map_1[1] = 1.0f;

		for(UINT i = 2 ; i<=iHeader.blocksize_1 ; i<<=1){
			for(UINT j = 0 ; j<i ; ++j){
				cos_map_1[i+j] = 1.0f/(2.0f*cos(3.141592f*((2.0f*j)+1.0f+N2)/(2.0f*i)));
			}
		}
	}

	if(b){
		INT64 t_pos = 0;
		UINT page_size = 0;

		while(offset<size && b){
			signature[0] = buf[offset+0];
			signature[1] = buf[offset+1];
			signature[2] = buf[offset+2];
			signature[3] = buf[offset+3];

			if(signature[0] != 'O' || signature[1] != 'g' || 
			   signature[2] != 'g' || signature[3] != 'S'){
				b = false; 
				break;
			}

			INT64 cur_granule = *((INT64*)&(buf[offset+6]));
			page_seg = buf[offset+26];

			offset += 27;
			UINT seg_offset = offset;
			offset += page_seg;
			UINT page_offset = offset;

			for(UINT i = 0 ; i<page_seg ; ++i){
				for( ; i<page_seg ; ++i){
					page_size += buf[seg_offset+i];
					if(buf[seg_offset+i] != 0xFF){break;}
				}
				bool check = VorbisDecode_Packet(&(buf[page_offset]), page_size, iHeader, sHeader, cos_map_0, cos_map_1, cur_vWin);
				offset += page_size;
				page_offset += page_size;
				page_size = 0;

				if(check){
					if(prev_vWin.block != nullptr){
						//overlap
						FLOAT* cur_Win	 = (!cur_vWin.mode_blockflag || !prev_vWin.mode_blockflag)?b0_Win:b1_Win;
						UINT cur_WinSize = (!cur_vWin.mode_blockflag || !prev_vWin.mode_blockflag)?iHeader.blocksize_0:iHeader.blocksize_1;

						UINT p_size = prev_vWin.block_size/iHeader.channel;
						UINT c_size =  cur_vWin.block_size/iHeader.channel;

						UINT p_center = (prev_vWin.block_size/iHeader.channel)>>1;
						UINT c_center = ( cur_vWin.block_size/iHeader.channel)>>1;

						UINT p_quarter = (prev_vWin.block_size/iHeader.channel)>>2;
						UINT c_quarter = ( cur_vWin.block_size/iHeader.channel)>>2;

						UINT r_st = p_center;
						UINT l_st = 0;
						UINT l_ed = c_center;

						//short + long
						if(!prev_vWin.mode_blockflag && cur_vWin.mode_blockflag){
							l_st = c_quarter-p_quarter;
							l_ed = c_quarter+p_quarter;
						}
						//long + short
						if(prev_vWin.mode_blockflag && !cur_vWin.mode_blockflag){
							r_st = (p_quarter*3)-c_quarter;
						}

						for(UINT j = p_center ; j<r_st && t_pos<cur_granule ; ++j){
							for(UINT ch = 0 ; ch<iHeader.channel ; ++ch){
								FLOAT float_pcm = prev_vWin.block[(ch*p_size)+j];
								if(float_pcm < -1.0f){float_pcm = -1.0f;}
								if(float_pcm >  1.0f){float_pcm =  1.0f;}

								INT16 data16 = float_pcm*((float_pcm>=0.0f)?0x7FFF:0x8000);
								INT idx = (t_pos*iHeader.channel)+ch;
								sound_data.buf[(idx*2)+1] = data16&0xFF;
								sound_data.buf[(idx*2)+0] = (data16&0xFF00)>>8;
							}
							++t_pos;
						}

						for(UINT j = 0 ; j<cur_WinSize>>1 && t_pos<cur_granule ; ++j){
							for(UINT ch = 0 ; ch<iHeader.channel ; ++ch){
								FLOAT prev = prev_vWin.block[(ch*p_size)+(r_st+j)]*cur_Win[(cur_WinSize>>1)+j];
								FLOAT next =  cur_vWin.block[(ch*c_size)+(l_st+j)]*cur_Win[j];
								FLOAT float_pcm = prev+next;
								if(float_pcm < -1.0f){float_pcm = -1.0f;}
								if(float_pcm >  1.0f){float_pcm =  1.0f;}

								INT16 data16 = float_pcm*((float_pcm>=0.0f)?0x7FFF:0x8000);
								INT idx = (t_pos*iHeader.channel)+ch;
								sound_data.buf[(idx*2)+1] = data16&0xFF;
								sound_data.buf[(idx*2)+0] = (data16&0xFF00)>>8;
							}

							++t_pos;
						}

						for(UINT j = l_ed ; j<c_center && t_pos<cur_granule ; ++j){
							for(UINT ch = 0 ; ch<iHeader.channel ; ++ch){
								FLOAT float_pcm = cur_vWin.block[(ch*c_size)+j];
								if(float_pcm < -1.0f){float_pcm = -1.0f;}
								if(float_pcm >  1.0f){float_pcm =  1.0f;}

								INT16 data16 = float_pcm*((float_pcm>=0.0f)?0x7FFF:0x8000);
								INT idx = (t_pos*iHeader.channel)+ch;
								sound_data.buf[(idx*2)+1] = data16&0xFF;
								sound_data.buf[(idx*2)+0] = (data16&0xFF00)>>8;
							}
							++t_pos;
						}

						delete[] prev_vWin.block;
						prev_vWin.block = nullptr;
					}

					prev_vWin.mode_blockflag	= cur_vWin.mode_blockflag;
					prev_vWin.prev_window_flag	= cur_vWin.prev_window_flag;
					prev_vWin.next_window_flag	= cur_vWin.next_window_flag;
					prev_vWin.block				= cur_vWin.block;
					prev_vWin.block_size		= cur_vWin.block_size;

					cur_vWin = {false, false, false, 0, nullptr};
				}
			}
		}
	}

	if(cos_map_0 != nullptr){delete[] cos_map_0;}
	if(cos_map_1 != nullptr){delete[] cos_map_1;}
	if(b0_Win != nullptr){delete[] b0_Win;}
	if(b1_Win != nullptr){delete[] b1_Win;}
	if(cur_vWin.block != nullptr){delete[] cur_vWin.block;}
	if(prev_vWin.block != nullptr){delete[] prev_vWin.block;}
	VorbisRelease_sHeader(sHeader);

	if(!b){
		if(sound_data.buf != nullptr){
			delete[] sound_data.buf;
			sound_data.buf = nullptr;
			sound_data.size = 0;
		}
		return false;
	}
	else{return true;}
}
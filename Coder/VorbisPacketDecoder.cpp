#include "VorbisPacket.h"
#include "VorbisSupporter.h"

#include "..\\Common\\Sort.h"

#define next_bit {bit = buf[++pos]; bit_pos = 0;}
#define bit_copy(c, a) {c = 0; for(INT b = 0 ; b<a ; ++b){c |= (bit&0x01)<<b; bit>>=1; ++bit_pos; if(bit_pos==8){next_bit;}}}
#define bit_shift(a) {for(INT b = 0 ; b<a ; ++b){bit>>=1; ++bit_pos; if(bit_pos==8){next_bit;}}}

#define setter buf, size, pos, bit, bit_pos
#define getter BYTE*& buf, UINT& size, UINT& pos, BYTE& bit, UINT& bit_pos

INT read_codebook_slow(getter, vorbis_codebook& c){
	UINT copy_bit = 0;
	UINT t_len = 0;
	UINT t_bit = 0;
	vorbis_codebook::block* code = c.code;
	UINT code_size = c.code_size;
	while(true){
		bit_copy(copy_bit, 1);
		t_bit <<= 1;
		t_bit |= copy_bit;
		++t_len;
		for(UINT i = 0 ; i<code_size ; ++i){
			if(t_bit == code[i].word && t_len == code[i].length){
				return i;
			}
		}
	}

	return -1;
}
INT read_codebook(getter, vorbis_codebook& c){
	UINT copy_bit = 0;
	UINT t_len = 0;
	UINT t_bit = 0;
	vorbis_codebook::block* code = c.code;
	UINT code_size = c.code_size;
	UINT i = 0;
	for( ; i<code_size && code[i].length == t_len ; ++i){}
	while(true){
		bit_copy(copy_bit, 1);
		t_bit <<= 1;
		t_bit |= copy_bit;
		++t_len;
		for( ; i<code_size && code[i].length == t_len ; ++i){
			if(t_bit == code[i].word){
				return code[i].entry;
			}
		}
	}

	return -1;
}

FLOAT read_VQ_table1(INT lookup_offset, FLOAT& last, INT& index_divisor, vorbis_codebook& c){
	FLOAT r = 0.0f;

	//loop first
	//last = 0
	//index_divisor = 1;

	UINT multiplicand_offset = (lookup_offset/index_divisor)%c.lookup_values;
	r = c.multiplicands[multiplicand_offset] * c.delta + c.minimum + last;
	if(c.sequence_p){last = r;}
	index_divisor *= c.lookup_values;

	return r;
}
FLOAT read_VQ_table2(INT lookup_offset, FLOAT& last, INT& multiplicand_offset, vorbis_codebook& c){
	FLOAT r = 0.0f;

	//loop first
	//last = 0
	//multiplicand_offset = lookup_offset*c.dimensions;

	r = c.multiplicands[multiplicand_offset] * c.delta + c.minimum + last;
	if(c.sequence_p){last = r;}
	++multiplicand_offset;
	
	return 0.0f;
}

//unused
bool floor0_packet_decode(getter, vorbis_floor& f, Setup_Header& sHeader){
	/*UINT amplitude = 0;
	bit_copy(amplitude, f.amplitude_bits);

	FLOAT* coeficients = new FLOAT[f.order];

	if(amplitude > 0){
		UINT booknumber = 0;
		bit_copy(booknumber, ilog(f.number_of_books));

		if(booknumber > sHeader.cb_size){return false;}

		FLOAT last = 0.0f;
		FLOAT* temp_vector = sHeader.cb[booknumber].vq_table;
		UINT temp_vector_size = sHeader.cb[booknumber].vq_size;
		for(UINT i = 0, length = 0 ; length<f.order ; ++i){
			for(UINT j = 0 ; j<temp_vector_size || length<f.order ; ++j, ++length){
				coeficients[(i*temp_vector_size)+j] = temp_vector[j] + last;
			}
			last = coeficients[(i*temp_vector_size)+(temp_vector_size-1)];
		}
	}

	delete[] coeficients;*/

	//curve computation

	return true;
}

bool floor1_packet_decode(getter, vorbis_floor& f, UINT*& y_list, UINT& max_y, Setup_Header& sHeader){
	UINT copy_bit = 0;

	bit_copy(copy_bit, 1);
	if(copy_bit == 0){return false;}

	UINT range = 0;
	switch(f.multiplier){
	case 1:	range = 256;	break;
	case 2:	range = 128;	break;
	case 3:	range = 86;		break;
	case 4:	range = 64;		break;
	}

	max_y = 2;
	for(UINT i = 0 ; i<f.partitions ; ++i){max_y += f.c_dim[f.partition_cl[i]];}

	y_list = new UINT[max_y];

	bit_copy(y_list[0], ilog(range-1));
	bit_copy(y_list[1], ilog(range-1));

	UINT floor1_y_values = 2;

	for(UINT i = 0 ; i<f.partitions ; ++i){
		UINT c = f.partition_cl[i];
		UINT cdim = f.c_dim[c];
		UINT cbits = f.sub_c[c];
		UINT csub = (1<<cbits)-1;
		UINT cval = 0;

		if(cbits > 0){
			cval = read_codebook(setter, sHeader.cb[f.mst_books[c]]);
		}

		for(UINT j = 0 ; j<cdim ; ++j){
			INT book = f.sub_books[(c*8)+(cval&csub)];
			cval >>= cbits;

			if(book >= 0){
				INT t = read_codebook(setter, sHeader.cb[book]);
				y_list[floor1_y_values++] = t;
			}
			else{
				y_list[floor1_y_values++] = 0;
			}
		}
	}

	return true;
}

bool floor1_packet_inverse(vorbis_floor& f, UINT*& y_list, UINT& max_y, FLOAT* output, UINT& window_size){
	UINT range = 0;
	switch(f.multiplier){
	case 1:	range = 256;	break;
	case 2:	range = 128;	break;
	case 3:	range = 86;		break;
	case 4:	range = 64;		break;
	}
	
	//inverse1
	struct SORT_STRUCT{
		INT x;
		INT y;
		bool step2_flag;
	}*arr = new SORT_STRUCT[max_y];
	for(UINT i = 0 ; i<max_y ; ++i){
		arr[i].x = f.x_list[i];
		arr[i].y = 0;
		arr[i].step2_flag = false;
	}

	arr[0].step2_flag = true;
	arr[1].step2_flag = true;
	arr[0].y = y_list[0];
	arr[1].y = y_list[1];
	for(UINT i = 2 ; i<max_y ; ++i){arr[i].y = 0;}

	for(UINT i = 2 ; i<max_y ; ++i){
		UINT low_n = low_neighbor(f.x_list, i);	//low_neighbor_offset
		UINT high_n = high_neighbor(f.x_list, i);	//high_neighbor_offset

		INT predicted = render_point(arr[low_n].x, arr[low_n].y, arr[high_n].x, arr[high_n].y, arr[i].x);
		UINT val = y_list[i];
		INT highroom = range-predicted;
		INT lowroom = predicted;

		INT room = 0;
		if(highroom < lowroom){room = highroom*2;}
		else{room = lowroom*2;}

		if(val != 0){
			arr[low_n].step2_flag = true;
			arr[high_n].step2_flag = true;
			arr[i].step2_flag = true;

			if(val >= room){
				if(highroom > lowroom){arr[i].y = val-lowroom+predicted;}
				else{arr[i].y = predicted-val+highroom-1;}
			}
			else{
				if(val&0x1){arr[i].y = predicted-((val+1)/2);}
				else{arr[i].y = predicted+(val/2);}
			}
		}
		else{
			arr[i].step2_flag = false;
			arr[i].y = predicted;
		}
	}

	//inverse2
	bool (*func)(SORT_STRUCT, SORT_STRUCT) = [](SORT_STRUCT a, SORT_STRUCT b)->bool{return a.x<b.x;};
	Sort::GetSingleton()->Quick(arr, arr+max_y, func);

	INT* floor1_y_final = new INT[window_size];
	for(UINT i = 0 ; i<window_size ; ++i){floor1_y_final[i] = 0;}

	INT hx = 0;
	INT lx = 0;
	INT ly = arr[0].y*f.multiplier;
	INT hy = 0;

	for(INT i = 1 ; i<max_y ; ++i){
		if(arr[i].step2_flag){
			hy = arr[i].y*f.multiplier;
			hx = arr[i].x;
			render_line(lx, ly, hx, hy, floor1_y_final);
			lx = hx;
			ly = hy;
		}
	}

	if(hx < window_size){render_line(hx, hy, window_size, hy, floor1_y_final);}

	//inverse dB
	for(UINT i = 0 ; i<window_size ; ++i){
		output[i] *= floor1_inverse_dB_table(floor1_y_final[i]);
	}

	delete[] arr;
	delete[] floor1_y_final;
	delete[] y_list;

	y_list = nullptr;

	return true;
}

//unused
bool residue01_packet_decode(getter, UINT cur_block_size, UINT ch, bool* do_not_decode_flag, FLOAT**& output, vorbis_residue& r, Setup_Header& sHeader){
	/*UINT actual_size = cur_block_size;
	
	UINT limit_begin = (r.begin<actual_size)?r.begin:actual_size;
	UINT limit_end = (r.end<actual_size)?r.end:actual_size;

	UINT classword_per_codeword = sHeader.cb[r.classbook].dimensions;
	UINT n_to_read = limit_end-limit_begin;
	UINT partition_to_read = n_to_read/r.partition_size;

	if(n_to_read == 0){return false;}

	UINT len = ch*partition_to_read;
	INT* classifications = new INT[len];
	for(INT i = 0 ; i<len ; ++i){classifications[i] = -1;}

	FLOAT* residue_result = new FLOAT[ch*classword_per_codeword*r.partition_size];

	for(UINT pass = 0 ; pass<8 ; ++pass){
		UINT count = 0;
		while(count<partition_to_read){
			if(pass == 0){
				for(UINT i = 0 ; i<ch ; ++i){
					if(do_not_decode_flag[i]){continue;}
					INT classword = read_codebook(setter, sHeader.cb[r.classbook]);
					for(UINT j = 0 ; j<classword_per_codeword ; ++j){
						if((j+count) >= partition_to_read){continue;}
						classifications[(i*partition_to_read)+(j+count)] = classword%r.classifications;
						classword /= r.classifications;
					}
				}
			}
			for(UINT j = 0 ; j<classword_per_codeword ; ++j){
				if(count >= partition_to_read){continue;}
				for(UINT i = 0 ; i<ch ; ++i){
					if(do_not_decode_flag[i]){continue;}

					UINT vq_class = classifications[(i*partition_to_read)+count];
					if(vq_class == -1){continue;}

					INT t_idx = r.books[(vq_class*8)+pass];
					if(t_idx < 0 || t_idx >= sHeader.cb_size){continue;}

					vorbis_codebook& t_cb = sHeader.cb[t_idx];

					if(r.type == 0){
						UINT t_num = r.partition_size/t_cb.dimensions;
						for(UINT k = 0 ; k<t_num ; ++k){
							read_codebook(setter, t_cb);
						}
					}
					else if(r.type == 1){
						for(UINT num2 = 0 ; num2<r.partition_size ; ){
							UINT t_num = r.partition_size/t_cb.dimensions;
							read_codebook(setter, t_cb);
							for(UINT k = 0 ; k<t_num ; ++k){
								++num2;
							}
						}
					}
				}
				++count;
			}
		}
	}
	*/
	return true;
}

bool residue2_packet_decode(getter, UINT cur_block_size, UINT ch, bool* do_not_decode_flag, FLOAT**& output, vorbis_residue& r, Setup_Header& sHeader){
	UINT actual_size = cur_block_size*ch;
	UINT limit_begin = (r.begin<actual_size)?r.begin:actual_size;
	UINT limit_end = (r.end<actual_size)?r.end:actual_size;

	UINT classword_per_codeword = sHeader.cb[r.classbook].dimensions;
	UINT n_to_read = limit_end-limit_begin;
	UINT partition_to_read = n_to_read/r.partition_size;

	if(n_to_read == 0){return false;}

	bool do_not_decode = true;
	for(UINT i = 0 ; i<ch ; ++i){if(!do_not_decode_flag[i]){do_not_decode = false; break;}}
	if(do_not_decode){return false;}

	INT* classifications = new INT[partition_to_read];
	for(INT i = 0 ; i<partition_to_read ; ++i){classifications[i] = -1;}

	FLOAT* residue_result = new FLOAT[n_to_read];
	for(INT i = 0 ; i<n_to_read ; ++i){residue_result[i] = 0.0f;}

	for(UINT pass = 0 ; pass<8 ; ++pass){
		UINT count = 0;
		while(count<partition_to_read){
			if(pass == 0){
				INT classword = read_codebook(setter, sHeader.cb[r.classbook]);
				for(INT j = classword_per_codeword-1 ; j>=0 ; --j){
					if((j+count) >= partition_to_read){
						classword /= r.classifications;
						continue;
					}
					classifications[j+count] = classword%r.classifications;
					classword /= r.classifications;
				}
			}
			for(UINT j = 0 ; j<classword_per_codeword && count < partition_to_read ; ++j, ++count){
				INT t_idx = r.books[(classifications[count]*8)+pass];
				if(t_idx < 0 || t_idx >= sHeader.cb_size){continue;}

				vorbis_codebook& t_cb = sHeader.cb[t_idx];
				INT offset = count*r.partition_size;
				for(UINT num2 = 0 ; num2<r.partition_size ; ){
					INT lookup_idx = read_codebook(setter, t_cb);
					if(t_cb.lookup_type == 1){
						FLOAT last = 0.0f;
						INT index_divisor = 1;

						for(UINT k = 0 ; k<t_cb.dimensions && num2<r.partition_size ; ++k){
							residue_result[offset+num2] += read_VQ_table1(lookup_idx, last, index_divisor, t_cb);
							++num2;
						}
					}
					else if(t_cb.lookup_type == 2){
						FLOAT last = 0.0f;
						INT multiplicand_offset = lookup_idx*t_cb.dimensions;

						for(UINT k = 0 ; k<t_cb.dimensions && num2<r.partition_size ; ++k){
							residue_result[offset+num2] += read_VQ_table2(lookup_idx, last, multiplicand_offset, t_cb);
							++num2;
						}
					}
					else{num2 += t_cb.dimensions;}
				}
			}
		}
	}

	for(INT i = 0, count = 0 ; count<n_to_read ; ++i){
		for(INT j = 0 ; j<ch && count<n_to_read ; ++j){
			if(limit_begin <= ((i*ch)+j) && ((i*ch)+j) < limit_end){
				output[j][i] = residue_result[count++];
			}
		}
	}

	delete[] classifications;
	delete[] residue_result;

	return true;
}
//fixed fft
void IMDCT_dct4(FLOAT* mdct, UINT N2, UINT ch, vorbis_window& vWin){
	FLOAT pi = 3.141592f;
	FLOAT half_pi = pi*0.5f;
	UINT N = N2<<1;
	UINT N4 = N2>>1;
	UINT N3_4 = N4*3;

	vWin.block_size = N*ch;
	vWin.block = new FLOAT[N*ch];

	FLOAT* fft = new FLOAT[N2];
	FLOAT* dct4 = new FLOAT[N2];

	for(UINT c = 0 ; c<ch ; ++c){
		//DCT-IV = (-c'-d, a-b') = (A, B)
		//IMDCT = (a-b', b-a', c+d', d+c') = (B, -B', -A', -A)

		//DCT-IV
		for(UINT n = 0 ; n<N2 ; ++n){
			dct4[n] = 0.0f;
			for(UINT k = 0 ; k<N2 ; ++k){
				dct4[n] += mdct[(c*N2)+k]*cos((pi/N2)*(n+0.5f)*(k+0.5f));
			}
		}
		for(UINT n = 0 ; n<N4 ; ++n){
			vWin.block[(c*N)+n]		 =  dct4[n+N4];
			vWin.block[(c*N)+n+N4]	 = -dct4[N2-n-1];
			vWin.block[(c*N)+n+N2]	 = -dct4[N4-n-1];
			vWin.block[(c*N)+n+N3_4] = -dct4[n];
		}
	}
	
	delete[] dct4;
}

void IMDCT_slow(FLOAT* mdct, UINT N2, UINT ch, vorbis_window& vWin){
	FLOAT pi = 3.141592f;
	FLOAT half_pi = pi*0.5f;
	UINT N = N2<<1;
	UINT N4 = N2>>1;
	UINT N3_4 = N4*3;

	vWin.block_size = N*ch;
	vWin.block = new FLOAT[N*ch];

	for(UINT c = 0 ; c<ch ; ++c){
		for(UINT n = 0 ; n<N ; ++n){
			vWin.block[(c*N)+n] = 0.0f;
			for(UINT k = 0 ; k<N2 ; ++k){
				vWin.block[(c*N)+n] += mdct[(c*N2)+k]*cos((pi/N2)*(n+0.5f+N4)*(k+0.5f));	//cos~~
			}
			//vWin.block[(c*N)+n] /= N4;
		}
	}
}

//fft dct-iv imdct
void IMDCT(FLOAT* mdct, UINT N2, UINT ch, FLOAT* cos_map, vorbis_window& vWin){
	FLOAT pi = 3.141592f;
	FLOAT half_pi = pi*0.5f;
	UINT N = N2<<1;
	UINT N4 = N2>>1;
	UINT N3_4 = N4*3;

	vWin.block_size = N*ch;
	vWin.block = new FLOAT[N*ch];

	FLOAT* fft = new FLOAT[N];

	//1.0f/(2.0f*cos(pi*(2n+1+N2)/2m))
	for(UINT c = 0 ; c<ch ; ++c){
		FLOAT* ch_mdct = &(mdct[c*N2]);
		//before starting
		{
			bool fft_chk = false;
			FLOAT* fft_0 = &(fft[0]);
			FLOAT* fft_1 = &(fft[0+N2]);

			fft_0[0] = ch_mdct[0];
			fft_1[0] = ch_mdct[0];
			for(UINT k = 1 ; k<N2 ; ++k){
				if(k%2){//odd
					fft_0[N4+(k>>1)] = ch_mdct[k]+ch_mdct[k-1];
					fft_1[N4+(k>>1)] = ch_mdct[k]+ch_mdct[k-1];
				}
				else{//even
					fft_0[k>>1] = ch_mdct[k]+ch_mdct[k-1];
					fft_1[k>>1] = ch_mdct[k]+ch_mdct[k-1];
				}	
			}

			for(UINT sub = 1 ; (1<<sub)<N2 ; ++sub){
				UINT sub_N = N2>>sub;

				for(UINT i = 0 ; i<(1<<sub) ; ++i){
					FLOAT* t_fft = fft_chk?&(fft_0[i*sub_N]):&(fft_1[i*sub_N]);
					FLOAT* t_fft_sub = fft_chk?&(fft_1[i*sub_N]):&(fft_0[i*sub_N]);

					if(i%2){
						t_fft[0] = t_fft_sub[0];
						for(UINT k = 1 ; k<sub_N ; ++k){
							if(k%2){t_fft[(sub_N>>1)+(k>>1)] = t_fft_sub[k]+t_fft_sub[k-1];}
							else{t_fft[k>>1] = t_fft_sub[k]+t_fft_sub[k-1];}
						}
					}
					else{
						for(UINT k = 0 ; k<sub_N ; ++k){
							if(k%2){t_fft[(sub_N>>1)+(k>>1)] = t_fft_sub[k];}
							else{t_fft[k>>1] = t_fft_sub[k];}
						}
					}
				}

				fft_chk = !fft_chk;
			}

			if(fft_chk){
				for(INT k = 0 ; k<N2 ; ++k){
					fft[(k*2)+0] = fft_1[k];
					fft[(k*2)+1] = fft_1[k];
				}
			}
			else{
				for(INT k = N2-1 ; k>=0 ; --k){
					fft[(k*2)+0] = fft_0[k];
					fft[(k*2)+1] = fft_0[k];
				}
			}
		}

		//fast imdct
		for(UINT sub = 2 ; (1<<sub)<=N ; ++sub){
			UINT sub_n = 1<<sub;
			UINT k_loop = 1<<(sub-1);

			for(UINT n = 0 ; n<(N>>sub) ; ++n){
				for(UINT k = 0 ; k<k_loop ; ++k){
					FLOAT a = fft[(n*sub_n)+k];
					FLOAT b = fft[(n*sub_n)+k+k_loop]*cos_map[k_loop+k];

					fft[(n*sub_n)+k] = a+b;;
					fft[(n*sub_n)+k+k_loop] = a-b;
				}
			}
		}

		//after end
		for(UINT k = 0 ; k<N2 ; ++k){
			fft[k] *= cos_map[N+k];
			fft[k+N2] *= cos_map[N+k+N2];
		}

		for(UINT n = 0 ; n<N ; ++n){vWin.block[(c*N)+n] = fft[n];}
	}

	delete[] fft;
}

bool VorbisPacketDecode(BYTE* buf, UINT size, Identification_Header& iHeader, Setup_Header& sHeader, FLOAT* cos_map_0, FLOAT* cos_map_1, vorbis_window& vWindow){
	BYTE bit = buf[0];
	UINT bit_pos = 0;
	UINT pos = 0;
	
	UINT copy_bit = 0;

	UINT packet_type = 0;	//0 - audio
	bit_copy(packet_type, 1);
	if(packet_type != 0){return false;}

	UINT mode_number = 0;
	bit_copy(mode_number, ilog(sHeader.md_size-1));

	if(mode_number >= sHeader.md_size){return false;}

	vWindow.mode_blockflag = sHeader.md[mode_number].blockflag;
	UINT n = iHeader.blocksize_0;
	if(vWindow.mode_blockflag){
		n = iHeader.blocksize_1;
		bit_copy(vWindow.prev_window_flag, 1);
		bit_copy(vWindow.next_window_flag, 1);
	}
	UINT n2 = n>>1;
	
	vorbis_mapping& mp = sHeader.mp[sHeader.md[mode_number].mapping];

	FLOAT* mdct = new FLOAT[n2*iHeader.channel];
	UINT** y_list = new UINT*[iHeader.channel];
	UINT y_max = 0;
	bool* no_residue = new bool[iHeader.channel];
	bool* do_not_decode_flag = new bool[iHeader.channel];
	FLOAT** residue_output = new FLOAT*[iHeader.channel];

	for(INT i = 0 ; i<n2*iHeader.channel ; ++i){mdct[i] = 0.0f;}
	for(INT i = 0 ; i<iHeader.channel ; ++i){
		y_list[i] = nullptr;
		no_residue[i] = false;
		do_not_decode_flag[i] = false;
		residue_output[i] = nullptr;
	}

	for(INT i = 0 ; i<iHeader.channel ; ++i){
		UINT submap_number = (mp.mux != nullptr)?mp.mux[i]:0;
		UINT floor_number = mp.submap_floor[submap_number];

		bool chk = false;
		if(sHeader.fl[floor_number].type == 0){}
		else if(sHeader.fl[floor_number].type == 1){
			chk = floor1_packet_decode(setter, sHeader.fl[floor_number], y_list[i], y_max, sHeader);
		}

		if(chk){no_residue[i] = false;}
		else{no_residue[i] = true;}
	}


	for(INT i = 0 ; i<mp.coupling_staps ; ++i){
		if(!no_residue[mp.magnitude[i]] || !no_residue[mp.angle[i]]){
			no_residue[mp.magnitude[i]] = false;
			no_residue[mp.angle[i]] = false;
		}
	}

	for(INT i = 0 ; i<mp.submaps ; ++i){
		UINT ch = 0;
		for(INT j = 0 ; j<iHeader.channel ; ++j){
			INT mux = (mp.mux != nullptr)?mp.mux[j]:0;
			if(mux == i){
				if(no_residue[j]){do_not_decode_flag[ch] = true;}
				else{do_not_decode_flag[ch] = false;}
				residue_output[ch] = &(mdct[j*n2]);
				++ch;
			}
		}

		UINT residue_number = mp.submap_residue[i];
		UINT residue_type = sHeader.rs[residue_number].type;

		if(residue_type == 2){
			residue2_packet_decode(setter, n2, ch, do_not_decode_flag, residue_output, sHeader.rs[residue_number], sHeader);
		}
		else if(residue_type == 0 || residue_type == 1){
			residue01_packet_decode(setter, n2, ch, do_not_decode_flag, residue_output, sHeader.rs[residue_number], sHeader);
		}
	}

	for(UINT ch = 0 ; ch<iHeader.channel ; ++ch){
		if(!do_not_decode_flag[ch]){continue;}
		for(UINT i = 0 ; i<n2 ; ++i){mdct[(ch*n2)+i] = 0.0f;}
	}

	for(INT i = mp.coupling_staps-1 ; i>=0 ; --i){
		FLOAT* M = &(mdct[mp.magnitude[i]*n2]);
		FLOAT* A = &(mdct[mp.angle[i]*n2]);

		for(INT j = 0 ; j<n2 ; ++j){
			FLOAT new_M = 0;
			FLOAT new_A = 0;

			if(M[j] > 0){
				if(A[j] > 0)	{new_M = M[j];	new_A = M[j]-A[j];}
				else			{new_A = M[j];	new_M = M[j]+A[j];}
			}
			else{
				if(A[j] > 0)	{new_M = M[j];	new_A = M[j]+A[j];}
				else			{new_A = M[j];	new_M = M[j]-A[j];}
			}

			M[j] = new_M;
			A[j] = new_A;
		}
	}

	for(INT i = 0 ; i<iHeader.channel ; ++i){
		if(y_list[i] == nullptr){continue;}

		UINT submap_number = (mp.mux != nullptr)?mp.mux[i]:0;
		UINT floor_number = mp.submap_floor[submap_number];

		if(sHeader.fl[floor_number].type == 0){}
		else if(sHeader.fl[floor_number].type == 1){
			floor1_packet_inverse(sHeader.fl[floor_number], y_list[i], y_max, &(mdct[i*n2]), n2);
		}
	}

	//inverse MDCT
	IMDCT(mdct, n2, iHeader.channel, vWindow.mode_blockflag?cos_map_1:cos_map_0, vWindow);

	delete[] y_list;
	delete[] no_residue;
	delete[] do_not_decode_flag;
	delete[] residue_output;
	delete[] mdct;

	return true;
}

#undef next_bit
#undef bit_copy
#undef bit_shift

#undef setter
#undef getter
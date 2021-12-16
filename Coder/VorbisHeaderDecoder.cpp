#include "VorbisHeader.h"
#include "VorbisSupporter.h"

#define RELEASE_ARRAY(name) if(name != nullptr){delete[] name;}

void ReleaseHeader(Setup_Header& sHeader){
	if(sHeader.cb != nullptr){
		for(UINT i = 0 ; i<sHeader.cb_size ; ++i){
			RELEASE_ARRAY(sHeader.cb[i].code);
			RELEASE_ARRAY(sHeader.cb[i].multiplicands);
		}
		delete sHeader.cb;
	}

	if(sHeader.fl != nullptr){
		for(UINT i = 0 ; i<sHeader.fl_size ; ++i){
			//RELEASE_ARRAY(sHeader.fl[i].book_list);
			
			RELEASE_ARRAY(sHeader.fl[i].partition_cl);
			RELEASE_ARRAY(sHeader.fl[i].c_dim);
			RELEASE_ARRAY(sHeader.fl[i].sub_c);
			RELEASE_ARRAY(sHeader.fl[i].mst_books);
			RELEASE_ARRAY(sHeader.fl[i].sub_books);
			RELEASE_ARRAY(sHeader.fl[i].x_list);
		}
		delete[] sHeader.fl;
	}

	if(sHeader.rs != nullptr){
		for(UINT i = 0 ; i<sHeader.rs_size ; ++i){
			RELEASE_ARRAY(sHeader.rs[i].books);
		}
		delete[] sHeader.rs;
	}

	if(sHeader.mp != nullptr){
		for(UINT i = 0 ; i<sHeader.mp_size ; ++i){
			RELEASE_ARRAY(sHeader.mp[i].magnitude);
			RELEASE_ARRAY(sHeader.mp[i].angle);
			RELEASE_ARRAY(sHeader.mp[i].mux);
			RELEASE_ARRAY(sHeader.mp[i].submap_floor);
			RELEASE_ARRAY(sHeader.mp[i].submap_residue);
		}
		delete[] sHeader.mp;
	}

	if(sHeader.md != nullptr){delete[] sHeader.md;}
}

#undef RELEASE_ARRAY

#define next_bit {bit = buf[++pos]; bit_pos = 0;}
#define bit_copy(c, a) {c = 0; for(INT b = 0 ; b<a ; ++b){c |= (bit&0x01)<<b; bit>>=1; ++bit_pos; if(bit_pos==8){next_bit;}}}
#define bit_shift(a) {for(INT b = 0 ; b<a ; ++b){bit>>=1; ++bit_pos; if(bit_pos==8){next_bit;}}}

#define setter buf, size, pos, bit, bit_pos
#define getter BYTE*& buf, UINT& size, UINT& pos, BYTE& bit, UINT& bit_pos

bool VorbisCodebookHeaderDecoder(getter, Setup_Header& sHeader){
	UINT copy_bit = 0;

	UINT vorbis_codebook_count = 0;
	bit_copy(vorbis_codebook_count, 8);
	++vorbis_codebook_count;

	sHeader.cb = new vorbis_codebook[vorbis_codebook_count];
	sHeader.cb_size = vorbis_codebook_count;
	for(INT num = 0 ; num<vorbis_codebook_count ; ++num){
		sHeader.cb[num].dimensions = 0;
		sHeader.cb[num].code_size = 0;
		sHeader.cb[num].code = nullptr;

		sHeader.cb[num].lookup_type = 0;
		sHeader.cb[num].minimum = 0.0f;
		sHeader.cb[num].delta = 0.0f;
		sHeader.cb[num].sequence_p = false;
		sHeader.cb[num].lookup_values = 0;
		sHeader.cb[num].multiplicands = nullptr;
	}

	for(INT num = 0 ; num<vorbis_codebook_count ; ++num){
		bit_copy(copy_bit, 24);
		if(copy_bit != 0x564342){return false;}
		UINT cb_dimensions = 0;
		UINT cb_entries = 0;
		bit_copy(cb_dimensions, 16);
		bit_copy(cb_entries, 24);

		bool ordered = 0;
		bit_copy(ordered, 1);

		sHeader.cb[num].dimensions = cb_dimensions;
		sHeader.cb[num].code_size = cb_entries;
		sHeader.cb[num].code = new vorbis_codebook::block[cb_entries];

		vorbis_codebook::block*& code = sHeader.cb[num].code;
		if(!ordered){
			bool sparse = 0;
			bit_copy(sparse, 1);
			for(INT i = 0 ; i<cb_entries ; ++i){
				code[i].length = 0;
				if(sparse){
					bit_copy(copy_bit, 1);
					if(copy_bit == 1){
						bit_copy(code[i].length, 5);
						++(code[i].length);
					}
				}
				else{
					bit_copy(code[i].length, 5);
					++(code[i].length);
				}
			}
		}
		else{
			UINT cur_length = 0;
			bit_copy(cur_length, 5);
			++cur_length;

			for(INT i = 0 ; i<cb_entries ; ){
				UINT number = ilog(cb_entries-i);
				INT t_len = i+number;
				for( ; i<t_len || i<cb_entries ; ++i){
					code[i].length = cur_length;
				}
				++cur_length;
			}
		}

		bit_copy(sHeader.cb[num].lookup_type, 4);

		//type == 0 none table
		//type == 1 lookup table
		//type == 2 dynamic lookup table(?)

		if(sHeader.cb[num].lookup_type == 1 || sHeader.cb[num].lookup_type == 2){
			bit_copy(copy_bit, 32);
			sHeader.cb[num].minimum = float32_unpack(copy_bit);
			bit_copy(copy_bit, 32);
			sHeader.cb[num].delta = float32_unpack(copy_bit);
			UINT cb_value_bits = 0;
			bit_copy(cb_value_bits, 4);
			++cb_value_bits;
			bit_copy(sHeader.cb[num].sequence_p, 1);

			sHeader.cb[num].lookup_values = 0;
			if(sHeader.cb[num].lookup_type == 1){
				sHeader.cb[num].lookup_values = lookup1_values(cb_entries, cb_dimensions);
			}
			else if(sHeader.cb[num].lookup_type == 2){
				sHeader.cb[num].lookup_values = cb_entries * cb_dimensions;
			}

			sHeader.cb[num].multiplicands = new UINT[sHeader.cb[num].lookup_values];
			for(INT i = 0 ; i<sHeader.cb[num].lookup_values ; ++i){
				bit_copy(sHeader.cb[num].multiplicands[i], cb_value_bits);
			}
		}
	}

	return true;
}

bool VorbisTimeDomainTransformHeaderDecoder(getter){
	UINT copy_bit = 0;

	bit_copy(copy_bit, 6);
	++copy_bit;
	for(UINT i = 0 ; i<copy_bit ; ++i){
		UINT t = 0;
		bit_copy(t, 16);
		if(t != 0){return false;}
	}

	return true;
}

bool VorbisFloor0HeaderDecoder(getter, vorbis_floor& fl, Setup_Header& sHeader){
	/*UINT copy_bit = 0;

	bit_copy(fl.order, 8);
	bit_copy(fl.rate, 16);
	bit_copy(fl.bark_map_size, 16);
	bit_copy(fl.amplitude_bits, 6);
	bit_copy(fl.amplitude_offset, 8);
	bit_copy(fl.number_of_books, 4);
	++fl.number_of_books;

	fl.book_list = new UINT[fl.number_of_books];
	for(UINT i = 0 ; i<fl.number_of_books ; ++i){
		fl.book_list[i] = 0;
		bit_copy(fl.book_list[i], 8);
	}*/

	return true;
}
bool VorbisFloor1HeaderDecoder(getter, vorbis_floor& fl, Setup_Header& sHeader){
	UINT copy_bit = 0;

	fl.partitions = 0;
	bit_copy(fl.partitions, 5);
	INT maximum_class = -1;
	fl.partition_cl = new INT[fl.partitions];

	for(UINT i = 0 ; i<fl.partitions ; ++i){
		bit_copy(fl.partition_cl[i], 4);
		maximum_class = (maximum_class >= fl.partition_cl[i])?maximum_class:fl.partition_cl[i];
	}
	++maximum_class;

	fl.c_dim = new UINT[maximum_class];
	fl.sub_c = new UINT[maximum_class];
	fl.mst_books = new INT[maximum_class];
	fl.sub_books = new INT[maximum_class*8];

	for(INT i = 0 ; i<maximum_class ; ++i){
		bit_copy(fl.c_dim[i], 3);
		++fl.c_dim[i];
		bit_copy(fl.sub_c[i], 2);

		fl.mst_books[i] = 0;
		if(fl.sub_c[i] != 0){
			bit_copy(fl.mst_books[i], 8);
			if(fl.mst_books[i] >= sHeader.cb_size){return false;}
		}

		UINT t_max = 1<<fl.sub_c[i];
		for(INT j = 0 ; j<t_max ; ++j){
			bit_copy(fl.sub_books[(i*8)+j], 8);
			--fl.sub_books[(i*8)+j];
			if(fl.sub_books[(i*8)+j] < -1 || fl.sub_books[(i*8)+j] >= (INT)sHeader.cb_size){return false;}
		}
	}

	fl.multiplier = 0;
	bit_copy(fl.multiplier, 2);
	++fl.multiplier;

	UINT rangebits = 0;
	bit_copy(rangebits, 4);

	fl.x_size = 2;
	for(UINT i = 0 ; i<fl.partitions ; ++i){fl.x_size += fl.c_dim[fl.partition_cl[i]];}

	fl.x_list = new UINT[fl.x_size];

	fl.x_list[0] = 0;
	fl.x_list[1] = 1<<rangebits;

	UINT floor1_x_values = 2;

	for(UINT i = 0 ; i<fl.partitions ; ++i){
		UINT cur_cnumber = fl.partition_cl[i];
		for(UINT j = 0 ; j<fl.c_dim[cur_cnumber] ; ++j){
			bit_copy(fl.x_list[floor1_x_values], rangebits);
			++floor1_x_values;
		}
	}

	return true;
}

bool VorbisFloorHeaderDecoder(getter, Setup_Header& sHeader){
	UINT copy_bit = 0;
	bool error = false;

	UINT vorbis_floor_count = 0;
	bit_copy(vorbis_floor_count, 6);
	++vorbis_floor_count;

	sHeader.fl = new vorbis_floor[vorbis_floor_count];
	sHeader.fl_size = vorbis_floor_count;

	for(UINT i = 0 ; i<vorbis_floor_count ; ++i){
		sHeader.fl[i].type = 0;

		//floor0
		/*sHeader.fl[i].order = 0;
		sHeader.fl[i].rate = 0;
		sHeader.fl[i].bark_map_size = 0;
		sHeader.fl[i].amplitude_bits = 0;
		sHeader.fl[i].amplitude_offset = 0;
		sHeader.fl[i].number_of_books = 0;
		sHeader.fl[i].book_list = nullptr;*/

		//floor1
		sHeader.fl[i].partitions = 0;
		sHeader.fl[i].partition_cl = nullptr;
		sHeader.fl[i].c_dim = nullptr;
		sHeader.fl[i].sub_c = nullptr;
		sHeader.fl[i].mst_books = nullptr;
		sHeader.fl[i].sub_books = nullptr;
		sHeader.fl[i].multiplier = 0;
		sHeader.fl[i].x_list = nullptr;
		sHeader.fl[i].x_size = 0;
	}

	for(UINT i = 0 ; i<vorbis_floor_count ; ++i){
		bit_copy(sHeader.fl[i].type, 16);
		if(sHeader.fl[i].type == 0){
			if(!VorbisFloor0HeaderDecoder(setter, sHeader.fl[i], sHeader)){error = true; break;}
		}
		else if(sHeader.fl[i].type == 1){
			if(!VorbisFloor1HeaderDecoder(setter, sHeader.fl[i], sHeader)){error = true; break;}
		}
		else{error = true; break;}
	}

	if(error){return false;}
	else{return true;}
}

bool VorbisResidueHeaderDecoder(getter, Setup_Header& sHeader){
	UINT copy_bit = 0;

	UINT residue_count = 0;
	bit_copy(residue_count, 6);
	++residue_count;

	sHeader.rs = new vorbis_residue[residue_count];
	sHeader.rs_size = residue_count;

	for(UINT i = 0 ; i<residue_count ; ++i){
		sHeader.rs[i].type = 0;

		sHeader.rs[i].begin = 0;
		sHeader.rs[i].end = 0;
		sHeader.rs[i].partition_size = 0;
		sHeader.rs[i].classifications = 0;
		sHeader.rs[i].classbook = 0;
		sHeader.rs[i].books = nullptr;
	}

	for(UINT num = 0 ; num<residue_count ; ++num){
		bit_copy(sHeader.rs[num].type, 16);
		if(sHeader.rs[num].type > 2){return false;}

		bit_copy(sHeader.rs[num].begin, 24);
		bit_copy(sHeader.rs[num].end, 24);
		bit_copy(sHeader.rs[num].partition_size, 24);	++sHeader.rs[num].partition_size;
		bit_copy(sHeader.rs[num].classifications, 6);	++sHeader.rs[num].classifications;
		bit_copy(sHeader.rs[num].classbook, 8);

		UINT* residue_cascade = new UINT[sHeader.rs[num].classifications];

		for(UINT i = 0 ; i<sHeader.rs[num].classifications ; ++i){
			UINT high_bit = 0;
			UINT low_bit = 0;
			bool bit_flag = false;

			bit_copy(low_bit, 3);
			bit_copy(bit_flag, 1);

			if(bit_flag){bit_copy(high_bit, 5);}

			residue_cascade[i] = (high_bit*8)+low_bit;
		}

		sHeader.rs[num].books = new INT[sHeader.rs[num].classifications*8];

		for(UINT i = 0 ; i<sHeader.rs[num].classifications ; ++i){
			for(INT j = 0 ; j<8 ; ++j){
				bool flag = (residue_cascade[i]>>j) & 0x1;
				if(flag){bit_copy(sHeader.rs[num].books[(i*8)+j], 8);}
				else{sHeader.rs[num].books[(i*8)+j] = -1;}
			}
		}

		delete[] residue_cascade;
	}

	return true;
}
bool VorbisMappingHeaderDecoder(getter, Identification_Header& iHeader, Setup_Header& sHeader){
	UINT copy_bit = 0;

	UINT mapping_count = 0;
	bit_copy(mapping_count, 6);
	++mapping_count;

	sHeader.mp = new vorbis_mapping[mapping_count];
	sHeader.mp_size = mapping_count;

	for(UINT i = 0 ; i<mapping_count ; ++i){
		sHeader.mp[i].submaps = 0;
		sHeader.mp[i].coupling_staps = 0;
		sHeader.mp[i].magnitude = nullptr;
		sHeader.mp[i].angle = nullptr;
		sHeader.mp[i].mux = nullptr;
		sHeader.mp[i].submap_floor = nullptr;
		sHeader.mp[i].submap_residue = nullptr;
	}

	for(UINT i = 0 ; i<mapping_count ; ++i){
		bit_copy(copy_bit, 16);
		if(copy_bit != 0){return false;}
		else if(copy_bit == 0){
			sHeader.mp[i].submaps = 0;
			bit_copy(copy_bit, 1);
			if(copy_bit == 1){bit_copy(sHeader.mp[i].submaps, 4);}
			++sHeader.mp[i].submaps;

			sHeader.mp[i].coupling_staps = 0;
			sHeader.mp[i].magnitude = nullptr;
			sHeader.mp[i].angle = nullptr;
			bit_copy(copy_bit, 1);
			if(copy_bit == 1){
				bit_copy(sHeader.mp[i].coupling_staps, 8);
				++sHeader.mp[i].coupling_staps;

				sHeader.mp[i].magnitude = new UINT[sHeader.mp[i].coupling_staps];
				sHeader.mp[i].angle = new UINT[sHeader.mp[i].coupling_staps];

				UINT log_chan = ilog(iHeader.channel-1);

				for(UINT j = 0 ; j<sHeader.mp[i].coupling_staps ; ++j){
					bit_copy(sHeader.mp[i].magnitude[j], log_chan);
					bit_copy(sHeader.mp[i].angle[j], log_chan);
				}
			}

			bit_copy(copy_bit, 2);
			if(copy_bit != 0){return false;}

			if(sHeader.mp[i].submaps > 1){
				sHeader.mp[i].mux = new UINT[iHeader.channel];
				for(UINT j = 0 ; j<iHeader.channel ; ++j){
					bit_copy(sHeader.mp[i].mux[j], 4);
					if(sHeader.mp[i].mux[j] >= sHeader.mp[i].submaps){return false;}
				}
			}

			sHeader.mp[i].submap_floor = new UINT[sHeader.mp[i].submaps];
			sHeader.mp[i].submap_residue = new UINT[sHeader.mp[i].submaps];

			for(UINT j = 0 ; j<sHeader.mp[i].submaps ; ++j){
				bit_copy(copy_bit, 8);
				bit_copy(sHeader.mp[i].submap_floor[j], 8);
				bit_copy(sHeader.mp[i].submap_residue[j], 8);

				if(sHeader.mp[i].submap_floor[j] >= sHeader.fl_size){return false;}
				if(sHeader.mp[i].submap_residue[j] >= sHeader.rs_size){return false;}
			}
		}
	}

	return true;
}
bool VorbisModeHeaderDecoder(getter, Setup_Header& sHeader){
	UINT copy_bit = 0;

	UINT mode_count = 0;
	bit_copy(mode_count, 6);
	++mode_count;

	sHeader.md = new vorbis_modes[mode_count];
	sHeader.md_size = mode_count;

	for(UINT i = 0 ; i<mode_count ; ++i){
		sHeader.md[i].blockflag = false;
		sHeader.md[i].mapping = 0;
	}

	for(UINT i = 0 ; i<mode_count ; ++i){
		UINT vorbis_mode_windowtype = 0;
		UINT vorbis_mode_transformtype = 0;

		bit_copy(sHeader.md[i].blockflag, 1);
		bit_copy(vorbis_mode_windowtype, 16);
		bit_copy(vorbis_mode_transformtype, 16);
		bit_copy(sHeader.md[i].mapping, 8);

		if(vorbis_mode_windowtype != 0 || vorbis_mode_transformtype != 0){return false;}
		if(sHeader.md[i].mapping >= sHeader.mp_size){return false;}
	}

	bit_copy(copy_bit, 1);
	if(copy_bit == 0){return false;}
	else{return true;}
}

#undef next_bit
#undef bit_copy
#undef bit_shift

#undef setter
#undef getter
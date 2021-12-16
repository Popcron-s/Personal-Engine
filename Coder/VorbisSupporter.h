#pragma once

#include "..\\Common\\variable.h"

struct Identification_Header{
	BYTE channel;
	UINT samplerate;
	UINT blocksize_0;
	UINT blocksize_1;
};

struct vorbis_codebook{
	struct block{
		UINT entry;
		UINT word;
		UINT length;
	}*code;
	UINT dimensions;
	UINT code_size;

	UINT lookup_type;
	FLOAT minimum;
	FLOAT delta;
	bool sequence_p;
	UINT lookup_values;
	UINT* multiplicands;
};

struct vorbis_floor{
	UINT type;

	//use floor0
	/*UINT order;
	UINT rate;
	UINT bark_map_size;
	UINT amplitude_bits;
	UINT amplitude_offset;
	UINT number_of_books;
	UINT* book_list;*/

	//use floor1
	UINT partitions;
	INT* partition_cl;
	UINT* c_dim;
	UINT* sub_c;
	INT* mst_books;
	INT* sub_books;
	UINT multiplier;
	UINT* x_list;
	UINT x_size;
};

struct vorbis_residue{
	UINT type;

	UINT begin;
	UINT end;
	UINT partition_size;
	UINT classifications;
	UINT classbook;
	INT* books;
};

struct vorbis_mapping{
	UINT submaps;
	UINT coupling_staps;
	UINT* magnitude;
	UINT* angle;
	UINT* mux;
	UINT* submap_floor;
	UINT* submap_residue;
};

struct vorbis_modes{
	bool blockflag;
	UINT mapping;
};

struct Setup_Header{
	vorbis_codebook* cb;
	UINT cb_size;

	vorbis_floor* fl;
	UINT fl_size;

	vorbis_residue* rs;
	UINT rs_size;

	vorbis_mapping* mp;
	UINT mp_size;

	vorbis_modes* md;
	UINT md_size;
};

struct vorbis_window{
	bool mode_blockflag;
	bool prev_window_flag;
	bool next_window_flag;

	UINT block_size;
	FLOAT* block;
};

INT ilog(INT i);
FLOAT float32_unpack(UINT i);
UINT lookup1_values(UINT codebook_entries, UINT codebook_dimensions);
UINT low_neighbor(UINT* list, UINT offset);
UINT high_neighbor(UINT* list, UINT offset);

INT render_point(INT x0, INT y0, INT x1, INT y1, INT x);
void render_line(INT x0, INT y0, INT x1, INT y1, INT* y_list);

FLOAT floor1_inverse_dB_table(UINT i);
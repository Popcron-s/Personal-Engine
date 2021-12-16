#pragma once

#include "VorbisHeader.h"

bool VorbisPacketDecode(BYTE* buf, UINT size, Identification_Header& iHeader, Setup_Header& sHeader, FLOAT* cos_map_0, FLOAT* cos_map_1, vorbis_window& vWindow);

#pragma once

#include "..\\Common\\variable.h"
#include "VorbisSupporter.h"

bool VorbisDecode_iHeader(BYTE* buf, UINT size, Identification_Header& iHeader);
bool VorbisDecode_sHeader(BYTE* buf, UINT size, Identification_Header& iHeader, Setup_Header& sHeader);
void VorbisRelease_sHeader(Setup_Header& sHeader);

bool VorbisDecode_Packet(BYTE*buf, UINT size, Identification_Header& iHeader, Setup_Header& sHeader, FLOAT* cos_map_0, FLOAT* cos_map_1, vorbis_window& vWindow);
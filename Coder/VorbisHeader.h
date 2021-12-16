#pragma once

#include "..\\Common\\variable.h"
#include "VorbisSupporter.h"

#define getter BYTE*& buf, UINT& size, UINT& pos, BYTE& bit, UINT& bit_pos

void ReleaseHeader(Setup_Header& sHeader);
bool VorbisCodebookHeaderDecoder(getter, Setup_Header& sHeader);
bool VorbisTimeDomainTransformHeaderDecoder(getter);
bool VorbisFloorHeaderDecoder(getter, Setup_Header& sHeader);
bool VorbisResidueHeaderDecoder(getter, Setup_Header& sHeader);
bool VorbisMappingHeaderDecoder(getter, Identification_Header& iHeader, Setup_Header& sHeader);
bool VorbisModeHeaderDecoder(getter, Setup_Header& sHeader);

#undef getter
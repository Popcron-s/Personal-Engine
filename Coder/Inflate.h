#pragma once

#include "..\\Common\\variable.h"

bool Inflate(BYTE*& cbuf, UINT& csize, UINT& cpos, BYTE*& ucbuf, UINT& ucsize, UINT& ucpos);
bool DataUncompress(BYTE*& cbuf, UINT& csize, UINT& cpos, BYTE*& ucbuf, UINT& ucsize, UINT& ucpos);
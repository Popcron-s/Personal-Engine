#pragma once

#include "..\\Common\\variable.h"

bool TXTDecoder(BYTE* buf, UINT size, wchar_t*& text, UINT& len);
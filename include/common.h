#pragma once

#include "object_pool.h"


using std::cout;
using std::endl;

inline constexpr size_t MAX_BYTES = 256 * 1024;
inline constexpr size_t NFREELIST = 208;
inline constexpr size_t NPAGES = 129;
inline constexpr size_t PAGE_SHITF = 13;





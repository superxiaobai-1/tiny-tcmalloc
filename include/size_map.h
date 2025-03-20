#pragma once

#include <assert.h>
#include <time.h>

#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "common.h"

// 整体控制在最多10%左右的内碎片浪费
//[1,128]              8byte对齐       freelist[0,16)
//[128+1,1024]         16byte对齐      freelist[16,72)
//[1024+1,8*1024]      128byte对齐     freelist[72,128)
//[8*1024+1,64*1024]   1024byte对齐    freelist[128,184)
//[64*1024+1,256*1024] 8*1024byte对齐  freelist[184,208)

class SizeClass {
 public:
  static inline size_t round_up(size_t bytes, size_t align_num) {
    size_t align_size = 0;
    if (bytes % align_num != 0) {
      align_size = (bytes / align_num + 1) * align_num;
    } else {
      align_size = bytes;
    }
    return align_size;
  }

  static inline size_t RoundUp(size_t size) {
    size_t alignment;

    if (size <= 128) {
      alignment = 8;
    } else if (size <= 1024) {
      alignment = 16;
    } else if (size <= 8 * 1024) {
      alignment = 128;
    } else if (size <= 64 * 1024) {
      alignment = 1024;
    } else if (size <= 256 * 1024) {
      alignment = 8 * 1024;
    } else {
      alignment = 1 << PAGE_SHITF;
    }

    return round_up(size, alignment);
  }

  static inline size_t alignment_index(size_t bytes, size_t align_shift) {
    // 计算对齐后的字节数
    size_t alignment = 1;  // 初始化对齐因子为 1
    for (size_t i = 0; i < align_shift; ++i) {
      alignment *= 2;  // 通过循环计算 2^align_shift
    }

    // 计算对齐后的字节数
    size_t aligned_bytes = bytes + alignment - 1;

    // 计算索引
    size_t index = aligned_bytes / alignment;

    // 返回计算得到的索引
    return index - 1;
  }

  static inline size_t Index(size_t bytes) {
    assert(bytes <= MAX_BYTES);

    // 存储不同范围的字节数所对应的索引偏移量
    static int group_array[4] = {16, 56, 56, 56};
    if (bytes <= 128) {
      return alignment_index(bytes, 3);
    } else if (bytes <= 1024) {
      return alignment_index(bytes - 128, 4) + group_array[0];
    } else if (bytes <= 8 * 1024) {
      return alignment_index(bytes - 1024, 7) + group_array[1] + group_array[0];
    } else if (bytes <= 64 * 1024) {
      return alignment_index(bytes - 8 * 1024, 10) + group_array[2] +
             group_array[1] + group_array[0];
    } else if (bytes <= 256 * 1024) {
      return alignment_index(bytes - 64 * 1024, 13) + group_array[3] +
             group_array[2] + group_array[1] + group_array[0];
    } else {
      assert(false);
    }

    return -1;
  }

  static size_t NumMoveSize(size_t size) {
    assert(size > 0);
    size_t num = 256 * 1024 / size;

    if (num < 2) num = 2;
    if (num > 512) num = 512;
    return num;
  }

  static size_t NumMovePage(size_t size) {
    size_t num = NumMoveSize(size);

    size_t npage = (num * size) >> PAGE_SHITF;
    if (npage == 0) npage = 1;
    return npage;
  }
};

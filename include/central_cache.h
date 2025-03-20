#pragma once
#include "common.h"
#include "size_map.h"
#include "span.h"

class CentralCache
{
 public:
  static CentralCache* GetInstance() {
    std::call_once(init_flag, []() { instance_ = new CentralCache(); });
    return instance_;
  }

  Span* GetOneSpan(SpanList& list, size_t byte_size);
  size_t FetchRangeObj(void*& start, void*& end, size_t batch_num, size_t size);
  void ReleaseListToSpans(void* start, size_t size);
  void InitializeSpanMemory(Span* span, size_t size);

 private:
  static CentralCache* instance_;
  static std::once_flag init_flag;

  CentralCache() {}
  CentralCache(const CentralCache& c) = delete;

  SpanList span_lists_[NFREELIST];
};
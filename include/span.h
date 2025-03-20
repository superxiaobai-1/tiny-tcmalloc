#pragma once

#include <vector>
#include <thread>
#include <time.h>
#include <assert.h>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <atomic>
#include <map>
#include "common.h"

struct Span
{
  PAGE_ID page_id_ = 0;
  size_t n_ = 0;

  Span* next_ = nullptr;
  Span* prev_ = nullptr;

  size_t use_count_ = 0;

  void* free_list_ = nullptr;
  size_t obj_size_ = 0;
  bool is_use_ = false;
};

class SpanList
{
 public:
  SpanList() {
    // head_ = new Span;
    head_ = span_pool_.New();
    head_->next_ = head_;
    head_->prev_ = head_;
  }

  Span* Begin() { return head_->next_; }

  Span* End() { return head_; }

  bool Empty() { return head_ == head_->next_; }
  void Insert(Span* pos, Span* newSpan) {
    assert(pos);
    assert(newSpan);

    Span* prev = pos->prev_;
    newSpan->next_ = pos;
    pos->prev_ = newSpan;

    prev->next_ = newSpan;
    newSpan->prev_ = prev;
  }

  void PushFront(Span* span) { Insert(Begin(), span); }

  Span* PopFront() {
    Span* sp = head_->next_;
    /*head->next_ = sp->next_;
    sp->next_ = nullptr;
    return sp;*/
    Erase(sp);
    return sp;
  }

  void Erase(Span* pos) {
    assert(pos);
    assert(pos != head_);

    Span* prev = pos->prev_;
    Span* next = pos->next_;

    next->prev_ = prev;
    prev->next_ = next;
  }

private:
 Span* head_;
 ObjectPool<Span> span_pool_;

public:
 std::mutex mtx_;
};
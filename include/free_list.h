
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


static void*& NextObj(void* obj)
{
	return *(void**)obj;
}

class FreeList
{
public:
	void Push(void* obj)
	{
		assert(obj);
                NextObj(obj) = free_list_;
                free_list_ = obj;
                size_++;
        }

        void* Pop() {
          assert(free_list_);
          void* obj = free_list_;
          free_list_ = NextObj(free_list_);
          size_--;
          return obj;
        }

        void PushRange(void* start, void* end, size_t n) {
          NextObj(end) = free_list_;
          free_list_ = start;

          size_ += n;
        }

        void PopRange(void* start, void* end, size_t n) {
          assert(n <= size_);
          start = free_list_;
          end = start;

          for (size_t i = 0; i < n - 1; i++) {
            end = NextObj(end);
          }

          free_list_ = NextObj(end);
          NextObj(end) = nullptr;
          size_ -= n;
        }
        size_t& GetMaxSize() { return max_size_; }

        size_t Size() { return size_; }
        bool Empty() { return free_list_ == nullptr; }

       private:
        void* free_list_ = nullptr;
        size_t max_size_ = 1;
        size_t size_ = 0;
};


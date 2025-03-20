#include "central_cache.h"
#include "page_cache.h"
CentralCache* CentralCache::instance_ = nullptr;  // 类外实现，不用加static
std::once_flag CentralCache::init_flag;

// 获取一个非空的span
Span* CentralCache::GetOneSpan(SpanList& list, size_t size)
{
	// 1、先去size 对应的桶里找非空的span
	Span* it = list.Begin();
	while (it != list.End())
	{
          if (it->free_list_ != nullptr) {
            return it;
          } else {
            it = it->next_;
          }
  }

        // 这里可以先将这个桶的桶锁进行解锁，其他线程可以释放内存对象回来不会阻塞
        list.mtx_.unlock();

        // 到这里说明，size对应的CentralCache层里面没有Span对象，那么需要向下一层申请
        // 先进行整个page cache层的加锁
        PageCache::GetInstance()->_pageMtx.lock();

        // 我们要计算向page cache层要多少页的span
        Span* span =
            PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(size));
        span->obj_size_ = size;

        span->is_use_ = true;

        // 解锁
        PageCache::GetInstance()->_pageMtx.unlock();

        // 申请回来的span对象进行切分，这会不需要加锁，因为这会其他线程访问不到这个span（还没挂到spanLists）

        // 要计算span的大块内存的起始地址和大块内存的大小
        // 页号左移PAGE_SHIFT就是地址
        char* start = (char*)(span->page_id_ << PAGE_SHITF);

        size_t bytes = span->n_ << PAGE_SHITF;
        char* end = start + bytes;

        // 先切一块做头
        span->free_list_ = start;
        start += size;

        // 进行尾插
        void* tail = span->free_list_;

        while (start < end) {
          NextObj(tail) = start;
          tail = NextObj(tail);
          start += size;
        }

        NextObj(tail) = nullptr;
        // 切好span，将span挂到桶里，加锁
        list.mtx_.lock();
        list.PushFront(span);

        return span;
}

// 从中心缓存获取一定数量的对象给thread cache
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t batch_num,
                                   size_t size) {
  // 先算size大小的对象，对应哪个桶，然后就去哪个桶去申请
  size_t index = SizeClass::Index(size);
  // 申请-加桶锁
  span_lists_[index].mtx_.lock();

  // 首先保证有一个非空的Span
  Span* span = GetOneSpan(span_lists_[index], size);
  assert(span);
  assert(span->free_list_);

  // 从span中获取batchNum个对象
  // 如果不够batchNum，有几个拿几个，返回实际拿到的个数

  start = span->free_list_;
  end = start;
  int actual_num = 1;
  size_t i = 0;
  while (i < batch_num && NextObj(end) != nullptr) {
    end = NextObj(end);
    ++i;
    ++actual_num;
  }

  // 拿走 actualNum个对象
  span->free_list_ = NextObj(end);
  NextObj(end) = nullptr;

  // 这里在释放的时候用
  span->use_count_ += actual_num;

  // 解锁
  span_lists_[index].mtx_.unlock();

  return actual_num;
}

void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	size_t index = SizeClass::Index(size);

        span_lists_[index].mtx_.lock();

        // 问题：换回来的内存挂到哪个span上？
        // 通过地址
        // start>>PAGE_SHIFT，就是span的页号，为了方便，我们可以建立页号和span指针的映射
        // 这里可以通过哈希表做- unordered_map<PAGE_ID, Span*>
        while (start) {
          void* next = NextObj(start);
          Span* span = PageCache::GetInstance()->MapObjectToSpan(start);

          // 头插进span的管理的链表
          NextObj(start) = span->free_list_;
          span->free_list_ = start;

          span->use_count_--;

          // span切分回去的小块内存都回来了
          // 将这个span 回收给page cache
          if (span->use_count_ == 0) {
            span_lists_[index].Erase(span);
            span->next_ = nullptr;
            span->next_ = nullptr;
            // span管理内存，通过页号和页数就可以
            span->free_list_ = nullptr;

            span_lists_[index].mtx_.unlock();

            PageCache::GetInstance()->_pageMtx.lock();
            PageCache::GetInstance()->ReleaseSpanToPageCache(span);

            PageCache::GetInstance()->_pageMtx.unlock();

            span_lists_[index].mtx_.lock();
          }

          start = next;
        }
        // 解锁
        span_lists_[index].mtx_.unlock();
}
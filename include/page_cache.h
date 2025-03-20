#pragma once

#include "common.h"
#include "object_pool.h"
#include "span.h"
#include "free_list.h"
class PageCache
{
public:
	
	static PageCache* GetInstance()
	{
          std::call_once(init_flag, []() { page_instance_ = new PageCache(); });
          return page_instance_;
        }

        Span* NewSpan(size_t k);

	Span* MapObjectToSpan(void* obj);

	void ReleaseSpanToPageCache(Span* span);

public:
	std::mutex _pageMtx;

       private:
        PageCache() {};
        PageCache(const PageCache&) = delete;

        std::map<PAGE_ID, Span*> _idSpanMap;

        ObjectPool<Span> span_pool_;

        SpanList span_lists_[NPAGES];

        static PageCache* page_instance_;
        static std::once_flag init_flag;
};
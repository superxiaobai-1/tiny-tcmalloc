#pragma once
#include "common.h"
#include "free_list.h"
class ThreadCache
{
public:

	void* Allocate(const size_t& size);

	void Deallocate(void* obj, size_t size);

	void* FetchFromCentralCache(size_t index, size_t size);

	void ListTooLong(FreeList& list, size_t size);

    static ThreadCache* GetThreadCache() {
        if (!thread_local_data_) {
            // thread_local_data_ = new ThreadCache();
            thread_local static ObjectPool<ThreadCache> tcPool;
            thread_local_data_ = tcPool.New();
        }
        return thread_local_data_;
    }
private:

	FreeList list_[NFREELIST];
	static thread_local ThreadCache* thread_local_data_;
};



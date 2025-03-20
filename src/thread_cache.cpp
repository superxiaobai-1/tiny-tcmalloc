#include "thread_cache.h"
#include "central_cache.h"
thread_local ThreadCache* ThreadCache::thread_local_data_ =nullptr;

void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	size_t batch_num = std::min(SizeClass::NumMoveSize(size), list_[index].GetMaxSize());

	if (list_[index].GetMaxSize() == batch_num)
	{
		list_[index].GetMaxSize() += 1;
	}

	void* start;
	void* end;

	size_t actual_num = CentralCache::GetInstance()->FetchRangeObj(start, end, batch_num, size);
	assert(actual_num >= 1);

	if (actual_num == 1)
	{
		assert(start == end);
		return start;
	}
	else
	{
		list_[index].PushRange(NextObj(start), end, actual_num-1);
		return start;
	}
        return nullptr;
}

void* ThreadCache::Allocate(const size_t& size)
{
	assert(size <= MAX_BYTES); 

	size_t alignSize = SizeClass::RoundUp(size);
	size_t index = SizeClass::Index(size);

	if (!list_[index].Empty())
	{
		return list_[index].Pop();
	}
	else
	{
		return FetchFromCentralCache(index, alignSize);
	}

}

void ThreadCache::Deallocate(void* obj, size_t size)
{
	assert(obj);
	assert(size <= MAX_BYTES);

	size_t index = SizeClass::Index(size);
	list_[index].Push(obj);

	if (list_[index].Size() >= list_[index].GetMaxSize())
	{
		ListTooLong(list_[index], size);
	}
}

void ThreadCache::ListTooLong(FreeList& list, size_t size)
{
	void* start = nullptr;
	void* end = nullptr;

	list.PopRange(start, end, list.GetMaxSize());

	CentralCache::GetInstance()->ReleaseListToSpans(start, size);
}

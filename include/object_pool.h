#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>

typedef unsigned long long PAGE_ID;

// 系统内存分配函数
inline static void* SystemAlloc(size_t kpage) {
    void* ptr = mmap(0, kpage << 13, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == nullptr)
        throw std::bad_alloc();
    return ptr;
}

// 系统内存释放函数
inline static void SystemFree(void* ptr, size_t kpage) {
    munmap(ptr, kpage << 13);
}

// 自由链表节点结构体
template <class T>
struct alignas(alignof(T)) FreeListNode {
  T object;                       // 实际对象
  FreeListNode* next_ = nullptr;  // 指向下一个节点的指针
};

template<class T>
class ObjectPool {
public:
 ObjectPool(size_t initial_size = 128 * 1024) : remain_bytes_(initial_size) {
   memory_ = (char*)SystemAlloc(remain_bytes_ >> 13);
 }

 ~ObjectPool() {
   if (memory_) {
     SystemFree(memory_, remain_bytes_ >> 13);
   }
 }
    T* New() {
        T* obj = nullptr;
        if (free_list_) {
          // 从自由链表中获取对象
          obj = &free_list_->object;
          free_list_ = free_list_->next_;
        } else {
          // 从系统分配新的内存块
          if (remain_bytes_ < sizeof(FreeListNode<T>)) {
            remain_bytes_ = 128 * 1024;
            memory_ = (char*)SystemAlloc(remain_bytes_ >> 13);
          }
          obj = reinterpret_cast<T*>(memory_);
          memory_ += sizeof(FreeListNode<T>);
          remain_bytes_ -= sizeof(FreeListNode<T>);
        }

        new(obj) T;
        return obj;
    }

    void Delete(T* obj) { 
        if (obj == nullptr)
            return;

        // 调用析构函数销毁对象
        obj->~T();

        // 将对象加入自由链表
        FreeListNode<T>* node = reinterpret_cast<FreeListNode<T>*>(obj);
        node->next_ = free_list_;
        free_list_ = node;
    }

private:
 char* memory_ = nullptr;                // 当前分配内存块的指针
 size_t remain_bytes_ = 0;               // 剩余可用字节数
 FreeListNode<T>* free_list_ = nullptr;  // 自由链表头指针
};

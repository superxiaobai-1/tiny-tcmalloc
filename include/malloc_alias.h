#pragma once

#include <algorithm>
#include <atomic>


#define ALIAS(name) __attribute__((alias(#name)))

extern "C" {
void* malloc(size_t size)  ALIAS(ConcurrentAlloc);
void free(void* ptr)  ALIAS(ConcurrentFree);
}

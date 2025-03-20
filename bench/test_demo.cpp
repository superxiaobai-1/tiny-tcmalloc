#include "object_pool.h"
#include "concurrent_alloc.h"
// #include "malloc_alias.h"


void Alloc1()
{
	for (size_t i = 0; i < 5; ++i)
	{
		void* ptr = ConcurrentAlloc(6);
	}
}

void Alloc2()
{
	for (size_t i = 0; i < 5; ++i)
	{
		void* ptr = ConcurrentAlloc(7);
	}
}

void TLSTest()
{
	std::thread t1(Alloc1);

	std::thread t2(Alloc2);

	t1.join();
	t2.join();
}
void TestConcurrentAlloc1()
{
  void* p1 = ConcurrentAlloc(6);
    // void* p2 = ConcurrentAlloc(8);
    // void* p3 = ConcurrentAlloc(1);
    // void* p4 = ConcurrentAlloc(7);
    // void* p5 = ConcurrentAlloc(8);
    // void* p6 = ConcurrentAlloc(8);
    // void* p7 = ConcurrentAlloc(8);

  cout << p1 << endl;
    // cout << p2 << endl;
    // cout << p3 << endl;
    // cout << p4 << endl;
    // cout << p5 << endl;
	// sleep(1);
  ConcurrentFree(p1);
    // ConcurrentFree(p2);
    // ConcurrentFree(p3);
    // ConcurrentFree(p4);
    // ConcurrentFree(p5);
    // ConcurrentFree(p6);
    // ConcurrentFree(p7);
}

void TestConcurrentAlloc()
{
	void* p1 = ConcurrentAlloc(6);
	void* p2 = ConcurrentAlloc(2);
	void* p3 = ConcurrentAlloc(4);
	void* p4 = ConcurrentAlloc(8);
	void* p5 = ConcurrentAlloc(7);
}
int main() {
  // TLSTest();
  TestConcurrentAlloc1();

  return 0;
}

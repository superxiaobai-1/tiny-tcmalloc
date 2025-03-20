#include <iostream>
#include <thread>

class InnerClass {
    public:
        InnerClass() : count_(0) {}

        void increment() {
            count_ += 1;
              std::cout << std::this_thread::get_id()<< ": INter in thread " << count_ << "\n";
        }

        int getCount() const {

            return count_;
        }

    private:
        int count_; // 计数变量
    };




class MyClass {
public:
    MyClass(int id) : id_(id) {
        std::cout << "MyClass constructor called for thread " << id_ << "\n";
    }
    
    ~MyClass() {
        std::cout << "MyClass destructor called for thread " << id_ << "\n";
    }

    void doWork() {
        
        for(int i =0; i < 10; i++){
          a_ += 1;
        //   innerObject.increment();  // 内部类的计数加操作
          std::cout << std::this_thread::get_id() << ": Working in thread "
                    << a_ << "\n";
        }
        // std::cout << "Working in thread " << id_ << "\n";
    }

public:
int id_;
static int a_ ;
 InnerClass innerObject;  // 内部类对象
};

int MyClass::a_ = 1;
// 全局对象（共享）
thread_local MyClass mySharedObject(0);  // 共享对象，非线程局部

void threadFunction(int threadId) {
    // 每个线程使用同一个对象
    mySharedObject = MyClass(threadId); // 赋值操作可能导致未定义行为
    mySharedObject.doWork();
}

int main() {
    std::thread t1(threadFunction, 1);
    std::thread t2(threadFunction, 1);
    std::thread t3(threadFunction, 1);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

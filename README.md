# ThreadPool
ThreadPool implementation in C++. The main purpose of creating this project was to learn threadpool. The code is easy to read and understand.
If you don't know about threadpool. Read [here](https://en.wikipedia.org/wiki/Thread_pool)

## Compilation
Tested on Linux Machine. Compile using below command
```text
g++ -std=c++11 main.cpp -pthread
```

## Usage
1. Create a threadpool of three threads
```cpp
Tpool pool(3);
```
It will create three workers which will execute tasks submitted to the threadpool.

2. Queue a work. If we have a function accepting two integer arguments and returning their sum( e.g - int add(int, int)), we can queue it using
```cpp
auto fut1 = pool.enq(add, 100, 200);
```
You have to pass function name, followed by the arguments accepted by the function. No need to worry about the function signature.
It returns an appropriate future object(`future<int>` in the above case). You can use `auto` to catch the return future object from the threadpool without worrying about the return type.
To print the result, use `get()` method of future object.
```cpp
std::cout << "foo returned: " << fut1.get() << "\n";
```
Lets understand another example. Suppose we have a class `Test`
```cpp
struct Test{};
```
There are two functions - one accepts rvalue reference and another lvalue reference of Test object as args.
```cpp
void foo(Test&& obj);
void bar(Test& obj);
```
You can queue these functions in our threadpool and pass rvalue/lvalue. Threadpool will make sure that the value type of argument is preserved when executing the work.
```cpp
Test t;
pool.enq(bar, t);  //threadpool forwards arg as lvalue
pool.enq(foo, Test()); //threadpool forwards arg as rvalue
```
There are more examples in the file [main.cpp](main.cpp)
## Implementation Overview
Below is the threadpool class overview with its members.
```cpp
class Tpool {
using Task = std::function<void(void)>;
public:
    Tpool(std::size_t sz);
    ~Tpool();
	template<typename T, typename ... Args>
    auto enq(T&& func, Args&& ... args) -> std::future<decltype(func(std::forward<Args>(args)...))>();
private:
	void start();
	void stop() noexcept;

    std::queue<Task> q;
    std::vector<std::thread> workers;
    std::mutex mu;
    std::condition_variable cv;

    bool pstop = false;
    std::size_t sz;
};
```
## License
[**THE MIT LICENSE**](LICENSE) Copyright (c) 2020 Kumar Sourav


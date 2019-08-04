#include "thpool.h"


struct Test {};

void foo(Test&& obj)
{
	std::cout << "foo::rvalue\n";
}


void bar(Test& obj)
{
	std::cout << "bar::lvalue\n";
}

int main()
{
	/* create the thread pool of three threads */
	Tpool pool(3);

	/* Lets create some simple lambda functions */
	auto f1 = [](int a, int b) -> int{ 
				//std::cout << "f1 id: " << std::this_thread::get_id() << "\n";
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				int x = a+b;
				return x;
				};
	auto f2 = [] (int a) -> int { 
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				//std::cout << "f2 id: " << std::this_thread::get_id() << "\n";
				a += 10;
				return a;
				};
	auto f3 = [](int& ref) { 
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				//std::cout << "f3 called with param: " <<  ref << "\n"; ref = 100;
				};

	/* Get the future */
	int x = 100, y = 200, z = 300;
	//auto fut1 = pool.enq(f1, x, y);
	auto fut1 = pool.enq(f1, 100, 200);
	//auto fut2 = pool.enq(f2, z);
	auto fut2 = pool.enq(f2, 300);

	/* pass a value to f3 which will change it to 100 */
	int val = 200;
	auto fut3 = pool.enq(f3, val);

	/* sleep to get proper print, remove and check */	
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "f1 returned: " << fut1.get() << "\n";
	std::cout << "f2 returned: " << fut2.get() << "\n";

	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	/* print the changed value */
	std::cout << "new value: " << val << "\n";

	/* f4 accepts rvalue as param */
	auto f4 = [](int&& ref){ std::cout << "f4: " << ref << "\n";};
	auto fut4 = pool.enq(f4,1000);
	Test t;
	pool.enq(foo, Test());
	pool.enq(bar, t);
	return 0;
}

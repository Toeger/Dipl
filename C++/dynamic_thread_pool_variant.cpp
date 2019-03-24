// compile with for example g++ -std=c++17 dynamic_thread_pool_variant.cpp -pthread

#include "TSSTP/thread_pool.hpp"
#include <future>
#include <iostream>
#include <thread>

int main() {
	Thread_pool thread_pool;

	struct Data {
		void operator+=(int) {}
		operator const char *() const {
			return "result";
		}
	};

	Data data;		   // initialize the problem
	data += 1;		   // work on the problem
	thread_pool.push(  // add work for thread pool
		[&data] {	  // make data accessible to the thread-pool thread
			data += 2; // thread-pool thread works on problem
		});
	data += 3;		   // main thread works concurrently on the problem
	std::cout << data; // print solution
}

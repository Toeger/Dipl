// compile with for example g++ -std=c++17 static_thread_pool_variant.cpp -pthread

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

	Data data;					// initialize the problem
	data += 1;					// work on the problem
	std::promise<void> promise; // mechanism to communicate in a thread-safe way
	std::future<void> future = promise.get_future();
	thread_pool.push(			 // add work for thread pool
		[&data, &promise] {		 // make data accessible to the thread-pool thread
			data += 2;			 // thread-pool thread works on problem
			promise.set_value(); // notify the main thread that the work is complete
		});
	data += 3;		   // main thread works concurrently on the problem
	future.get();	  // wait for thread-pool thread to finish working on the problem
	data += 4;		   // main thread works alone on the problem
	std::cout << data; // print solution
}

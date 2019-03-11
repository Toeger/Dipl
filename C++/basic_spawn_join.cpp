//compile with for example g++ -std=c++17 -pthread basic_spawn_join.cpp

#include <iostream>
#include <thread>

int main() {
	struct Data {
		void operator+=(int) {}
		operator const char *() const {
			return "result";
		}
	};

	Data data;			//initialize the problem
	data += 1;			//work on the problem alone
	std::thread worker( //spawn a worker thread to help solve the problem
		[&data] {		// make data accessible to the worker thread
			data += 2;  //worker thread works on problem
		});
	data += 3;		   //main thread works concurrently on the problem
	worker.join();	 //wait for the worker thread to finish
	data += 4;		   //main thread works on the problem alone
	std::cout << data; //print solution
}

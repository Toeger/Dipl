//compile with for example g++ -std=c++17 -pthread manual_mutex.cpp

#include <iostream>
#include <mutex>
#include <thread>

int main() {
	using Data = int;

	Data data{};
	data += 1; //access without mutex protection
	std::mutex mutex;
	std::thread worker([&data, &mutex] {
		mutex.lock();
		data += 2; //access under mutex protection
		mutex.unlock();
		//data += 3; //no diagnostics
	});
	mutex.lock();
	data += 4; //access under mutex protection
	mutex.unlock();
	//data += 5; //no diagnostics
	worker.join();
	data += 6;		   //access without mutex protection
	std::cout << data; //access without mutex protection
}

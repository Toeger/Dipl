//compile with for example g++ -std=c++17 -pthread atomic.cpp

#include <atomic>
#include <thread>
#include <iostream>

int main() {
	std::atomic<int> data{0};
	std::thread thread1{[&data] {
		data++;
	}};
	std::thread thread2{[&data] {
		data++;
	}};
	thread1.join();
	thread2.join();
	std::cout << data << '\n';
}

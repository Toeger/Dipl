//compile with for example g++ -std=c++17 -pthread thread_local.cpp
//tested on g++ (Debian 8.2.0-20) 8.2.0

#include <thread>
#include <iostream>

int main() {
	thread_local int data = 0;
	std::thread thread1{[] {
		data = 1;
	}};
	std::thread thread2{[] {
		data = 2;
	}};
	thread1.join();
	thread2.join();
	std::cout << data << '\n'; //prints 0 because other threads cannot access this thread's data
}

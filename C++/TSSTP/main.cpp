#include <iostream>

#include "TSSTP.hpp"
#include "thread_pool.hpp"

int main() {
	Thread_pool thread_pool;
	using namespace TST;
	using Data = int;

	Data data{};                                                             //initialize the problem
	Thread_private<Data, Non_owner> tp_data = &data;                         //initialize owner pointer
	*tp_data += 1;                                                           //work on the problem alone
	Mutex_protected<Data, Non_owner, Waitable> mp_data = std::move(tp_data); //convert thread-private to mutex-protected ownership
	//tp_data now invalid
	thread_pool.push( //add work for thread pool
		[mp_data] {   // make data accessible to the thread-pool thread
			//mp_data is a copy of the original Mutex_protected pointer
			*mp_data.lock() += 2; //thread-pool thread works on problem
		});
	*mp_data.lock() += 3;                //main thread works concurrently on the problem
	tp_data = std::move(mp_data).wait(); //convert shared ownership back to unique ownership, waiting if necessary
	//tp_data is now valid again
	*tp_data += 4;                       //main thread works alone on the problem
	std::cout << *tp_data << std::flush; //print solution
}

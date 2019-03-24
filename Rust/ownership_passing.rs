//compile with for example rustc ownership_passing.rs
fn main() {
	let mut data = Vec::new();
	data.push(1); //work on problem
	let mutex = std::sync::Mutex::new(data); //mutex takes ownership of data
	//data.push(2); //error[E0382]: use of moved value: `data`
	mutex.lock().unwrap().push(3); //access under mutex protection
	let mut data = mutex.into_inner().unwrap(); //move vector out
	//mutex.lock().unwrap().push(4); //error[E0382]: use of moved value: `mutex`
	data.push(5); //access without mutex
}

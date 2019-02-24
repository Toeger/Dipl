//tested on rustc 1.34.0-nightly (e1c6d0057 2019-02-22) with crossbeam 0.7.1
//See ../how to build.txt

extern crate crossbeam;

fn main() {
    let data = std::sync::Mutex::new(0); //create mutex-protected data, in this case of an i32 with value 0

    crossbeam::scope(|scope| {
        let _thread1 = scope.spawn(|_| { //create thread
            *data.lock().unwrap() += 1; //access concurrently under mutex protection
        });
        let _thread2 = scope.spawn(|_| { //create thread
            *data.lock().unwrap() += 1; //access concurrently under mutex protection
        });
    }).unwrap();

    println!("{}", *data.lock().unwrap());
}

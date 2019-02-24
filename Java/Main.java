//compile with javac Main.java && java Main
//tested on openjdk version "1.8.0_181" and openjdk version "11.0.2" 2019-01-15
public class Main {
	public static class SynchronizedInt {
		private int i = 0; //automatically protect i
		public synchronized void increment() { //automatically locks mutex when called
			i++;
		}
		public synchronized int get_value() { //automatically locks same mutex when called
			return i;
		}
	}

	public static SynchronizedInt si = new SynchronizedInt(); //global instance of a SynchronizedInt
	public static void main(String[] args) {
		Runnable runner = new Runnable() {
			public void run() { //function to be executed in threads
				si.increment();
			}
		};
		Thread thread1 = new Thread(runner);
		Thread thread2 = new Thread(runner);
		thread1.start();
		thread2.start();
		try {
			thread1.join();
			thread2.join();
		} catch (InterruptedException ex) {
			System.err.println("Thread interrupted error");
			return;
		}
		System.out.println(si.get_value()); //prints 2
	}
}

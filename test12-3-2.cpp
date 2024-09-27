#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

const int BUFFER_SIZE = 5;
queue<int> buffer;
int count = 0;
mutex mtx;
condition_variable cv;

void producer() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
		
		cout<<"Start produce"<<endl;
        unique_lock<mutex> lock(mtx);
        
        cout<<"Wait produce"<<endl;
        cv.wait(lock, [] { return count < BUFFER_SIZE; }); //避免busy wait 等待過程中解鎖lock   這個執行緒會一直等待，直到有其他執行緒呼叫 cv.notify_all()
        /*while (count >= BUFFER_SIZE) {
            cout << "Buffer full" << endl;
            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(500));
            lock.lock();
        }*/
		cout<<"Wait produce finish"<<endl;
		
        int next_produced = rand() % 100 + 1;
        buffer.push(next_produced);
        count++;
        cout << "produce : " << next_produced << endl;
        cout << "buffer : ";
        queue<int> temp = buffer;
        while (!temp.empty()) {
            cout << temp.front() << " ";
            temp.pop();
        }
        cout << endl << endl;

		cout<<"End produce"<<endl; 
        lock.unlock();
        cv.notify_all();
    }
}

void consumer() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(2));
		
		cout<<"Start consume"<<endl; 
        unique_lock<mutex> lock(mtx);
        
        cout<<"Wait consume"<<endl;
        cv.wait(lock, [] { return count > 0; }); 
		cout<<"Wait consume finish"<<endl;
		
		
        int next_consumed = buffer.front();
        buffer.pop();
        count--;
        cout << "consume : " << next_consumed << endl;
        cout << "buffer : ";
        queue<int> temp = buffer;
        while (!temp.empty()) {
            cout << temp.front() << " ";
            temp.pop();
        }
        cout << endl << endl;

		cout<<"End consume"<<endl; 
        lock.unlock();
        cv.notify_all();     
    }
}



int main() {
    srand(time(nullptr));

    thread producer_thread(producer);
    thread consumer_thread(consumer);

    // Gracefully exit the program
    producer_thread.join();
    consumer_thread.join();

    return 0;
}

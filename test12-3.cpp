#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib> 
#include <ctime> 
#include <queue>
#include <mutex>
using namespace std;

const int BUFFER_SIZE = 5;
queue<int> buffer;
int count = 0;
int available_flag = 0;
std::mutex mtx;

void producer() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));

        std::unique_lock<std::mutex> lock(mtx);
        while (count >= BUFFER_SIZE || available_flag == 1) {
            if (count >= BUFFER_SIZE) {
                cout << "Buffer full" << endl;
            }
            else if (available_flag == 1) {
                cout << "Waiting for the other thread" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(500));
        }

        cout << "START produce" << endl;
        available_flag = 1;

        srand(time(NULL));
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
        cout << endl;

        cout << "END produce" << endl << endl;
        available_flag = 0;

        lock.unlock();
    }
}

void consumer() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(2));

        std::unique_lock<std::mutex> lock(mtx);
        while (count <= 0 || available_flag == 1) {
            if (count <= 0) {
                cout << "Buffer empty" << endl;
            }
            else if (available_flag == 1) {
                cout << "Waiting for the other thread" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(500));
        }

        cout << "START CONSUME" << endl;
        available_flag = 1;

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
        cout << endl;

        cout << "END CONSUME" << endl << endl;
        available_flag = 0;

        lock.unlock();
    }
}

int main() {
    srand(time(NULL));  // 初始化亂數種子

    thread producer_thread(producer);
    thread consumer_thread(consumer);

    this_thread::sleep_for(chrono::seconds(20));  // 讓程式執行20秒，然後結束

    producer_thread.join();
    consumer_thread.join();

    cout << "Main thread exiting." << endl;

    return 0;
}


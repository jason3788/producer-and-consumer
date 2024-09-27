#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <conio.h>

using namespace std;

const int BUFFER_SIZE = 5;  
queue<int> buffer;
int count = 0;
mutex mtx;
condition_variable cv;
int producer_sleep_seconds;
int consumer_sleep_seconds;
atomic<bool> exitFlag(false);

void producer() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(producer_sleep_seconds)); 
		
		cout<<"Start produce"<<endl;   
        unique_lock<mutex> lock(mtx);
        
        cout<<"Wait produce"<<endl;
        cv.wait(lock, [] { return count < BUFFER_SIZE || exitFlag.load(); });
		cout<<"Wait produce finish"<<endl;
		
        if (exitFlag.load()) {
            cout << "Exiting producer thread." << endl;
            break;
        }

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
        cout<<endl;
        cout<<"End produce"<<endl; 
        
        cout << endl << endl;
			
        lock.unlock();
        cv.notify_one();
    }
}

void consumer() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(consumer_sleep_seconds));
		cout<<"Start consume"<<endl; 
		
        unique_lock<mutex> lock(mtx);
        
        cout<<"Wait consume"<<endl;
        cv.wait(lock, [] { return count > 0 || exitFlag.load(); });
		cout<<"Wait consume finish"<<endl;
		
        if (exitFlag.load()) {
            cout << "Exiting consumer thread." << endl;
            break;
        }

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
        cout<<endl;
        cout<<"End consume"<<endl; 
        cout << endl << endl;

        lock.unlock();
        cv.notify_one();
    }
}

void keyboardInputThread() {
    while (true) {
        if (_kbhit()) {
        	cout<<"Detect char"<<endl; 
        	unique_lock<mutex> lock(mtx);
        	cout<<"Start input_thread"<<endl; 
            char ch = _getch();
            if (ch == 'q') {
                exitFlag.store(true);
                break;
            } 
			else if (ch == 'e') {
                while (!buffer.empty()) {
                    buffer.pop();
                }
                count = 0;
                cout << "Buffer cleared." << endl;
            }
            else if(ch == 's'){
            	while(true){
					if(_kbhit()){
						char check = _getch();
						if(check == 'c'){
							break;
						}
					}
            	}
			}
			else if (ch == 'c') {
    			cout << "輸入1改變producer速度；輸入2改變consumer速度" << endl;
    			while (true) {
		        	if (_kbhit()) {
			            char check = _getch();
			            if (check == '1' || check == '2') {
			                int new_speed; 
			                cout << "請輸入新的速度：" << endl;
			                cin >> new_speed;
			
			                if (check == '1') {
			                    producer_sleep_seconds = new_speed;
			                } else if (check == '2') {
			                    consumer_sleep_seconds = new_speed;
			                }
			
			                break;
			            }
			        }
			    }
			}

			cout<<"Finish input_thread"<<endl; 
			
			lock.unlock();
            cv.notify_one();
        }
    }
}

int main() {
    cout << "請輸入producer的速度(s)" << endl;
    cin >> producer_sleep_seconds;

    cout << "請輸入consumer的速度(s)" << endl;
    cin >> consumer_sleep_seconds;

    srand(time(nullptr));

    thread producer_thread(producer);
    thread consumer_thread(consumer);
    thread input_thread(keyboardInputThread);

    producer_thread.join();
    consumer_thread.join();
    input_thread.join();

    cout << "finish" << endl;
    return 0;
}


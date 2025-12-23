#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

enum State { NEW, READY, RUNNING, WAITING, TERMINATED };

string stateToStr(State s) {
    switch (s) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case WAITING: return "WAITING";
        case TERMINATED: return "TERMINATED";
    }
    return "";
}

class Semaphore {
    int value;
    mutex mtx;
public:
    Semaphore(int v = 1) : value(v) {}

    void wait() {
        while (true) {
            mtx.lock();
            if (value > 0) {
                value--;
                mtx.unlock();
                break;
            }
            mtx.unlock();
        }
    }

    void signal() {
        mtx.lock();
        value++;
        mtx.unlock();
    }
};

struct SimThread {
    int id;
    State state;
    SimThread(int i) : id(i), state(NEW) {}
};

vector<SimThread> threads;
queue<int> readyQ;
Semaphore sem(1);
int sharedCounter = 0;
ofstream logFile;

void log(int id, State s) {
    logFile << "Thread " << id << " -> " << stateToStr(s) << endl;
}

void executeThread(int id, bool useSemaphore) {
    threads[id].state = RUNNING;
    log(id, RUNNING);

    if (useSemaphore) {
        threads[id].state = WAITING;
        log(id, WAITING);
        sem.wait();
    }

    int temp = sharedCounter;
    this_thread::sleep_for(chrono::milliseconds(100));
    sharedCounter = temp + 1;

    if (useSemaphore) sem.signal();

    threads[id].state = TERMINATED;
    log(id, TERMINATED);
}

void manyToOne(bool useSemaphore) {
    while (!readyQ.empty()) {
        int id = readyQ.front();
        readyQ.pop();
        threads[id].state = READY;
        log(id, READY);
        executeThread(id, useSemaphore);
    }
}

void oneToMany(bool useSemaphore) {
    vector<thread> workers;
    while (!readyQ.empty()) {
        int id = readyQ.front();
        readyQ.pop();
        workers.emplace_back(executeThread, id, useSemaphore);
    }
    for (auto &t : workers) t.join();
}

int main() {
    logFile.open("output_log.txt");

    int n, model;
    bool useSemaphore;

    cout << "Enter number of threads: ";
    cin >> n;

    cout << "Choose Model:\n1. Many-to-One\n2. One-to-Many\nChoice: ";
    cin >> model;

    cout << "Use Semaphore? (1 = Yes, 0 = No): ";
    cin >> useSemaphore;

    for (int i = 0; i < n; i++) {
        threads.emplace_back(i);
        readyQ.push(i);
        log(i, NEW);
    }

    if (model == 1)
        manyToOne(useSemaphore);
    else
        oneToMany(useSemaphore);

    logFile << "Final Shared Counter Value: " << sharedCounter << endl;
    logFile.close();

    cout << "Simulation completed. Check output_log.txt\n";
    return 0;
}
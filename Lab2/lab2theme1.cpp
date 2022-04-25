#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include "UserInterruptHandler.h"
using namespace std;

//разделяемый ресурс - консоль, булевые переменные
void PrintIdThreadFunction(mutex &mtx, condition_variable &cv, int id, bool *notStopped)
{
    unique_lock<mutex> lck(mtx); //закрываем до разрешения

    while(*notStopped == true){

        cv.wait(lck); //ждем разблокировни


        if (*notStopped == false) //если запретили работать - завершаем цикл
        {
            break;
        }

        cout <<  id + 1 << endl; //печатаем и снова закрываемся

    }

    cout << "Thread " << id + 1 << " ended work" << endl;
}

void printDigit(condition_variable& cv)
{
    cv.notify_one();
}

int main()
{
    int n;
    cout << "Input number of threads: ";
    cin >> n;

    mutex mtx;
    condition_variable cv;

    bool *notStopped = new bool(true);
    thread* threads = new thread[n]; //массив потоков

    for (int id = 0; id < n; id++)
    {
        threads[id] = thread(PrintIdThreadFunction, ref(mtx), ref(cv), id, notStopped);
    }

    UserInterruptHandler intHandler;
    try
    {
        while(true)
        {

            intHandler.checkInterruptionAndThrow();

            printDigit(cv);

            this_thread::sleep_for(chrono::milliseconds(300));
        }

    }
    catch (const runtime_error &e)
    {

        cout << e.what() << endl;

        *notStopped = false; //выходим из цикла

        cv.notify_all();
    }


    for (int i = 0; i < n; i++)
    {
        threads[i].join();
    }

    return 0;
}
#include <iostream>
#include <vector>
#include <ctime>
#include <thread>
#include <algorithm>
#include <future>

using namespace std;

void InvervalSearch(vector<int> NumbersVector, int TasksNumber, int IntervalNumber, promise<int>&& PromiseMax, promise<int>&& PromiseMin)
{
    //инициализация
    int IntervalMax = INT_MIN;
    int IntervalMin = INT_MAX;

    //каждый интервал ищет на своем интервале
    for (int i = IntervalNumber * (NumbersVector.size() / TasksNumber); i < (IntervalNumber + 1) * (NumbersVector.size() / TasksNumber); i++)
    {

        //сравниваем
        if (NumbersVector[i] > IntervalMax)
        {
            IntervalMax = NumbersVector[i];
        }

        if (NumbersVector[i] < IntervalMin)
        {
            IntervalMin = NumbersVector[i];
        }

    }

    //передаем в промис значения
    PromiseMax.set_value(IntervalMax);
    PromiseMin.set_value(IntervalMin);
}

void FindMinMaxOnInterval(int n, vector<int> NumbersVector)
{
    chrono::time_point<chrono::steady_clock> Begin;
    chrono::time_point<chrono::steady_clock> End;
    chrono::duration<float> WorkingTime;

    int TasksNumber = n; //число потоков

    vector<thread> t;

    //для каждого из потоков создаем пары футуре-промис
    //то что обущали поулчить
    promise<int>* PromiseMax = new promise<int>[n];
    promise<int>* PromiseMin = new promise<int>[n];

    //что вернем потом
    future<int>* FutureMax = new future<int>[n];
    future<int>* FutureMin = new future<int>[n];

    //объявлем максимальные числа
    int MaxNumber = INT_MIN;
    int MinNumber = INT_MAX;

    //запускаем отсчет времени
    Begin = chrono::high_resolution_clock::now();

    for (int interval = 0; interval < n; interval++)
    {
        //возвращает футуре объект в футуре объект откуда мы обещали
        FutureMax[interval] = PromiseMax[interval].get_future();
        FutureMin[interval] = PromiseMin[interval].get_future();

        t.push_back(thread(InvervalSearch, NumbersVector, TasksNumber, interval, move(PromiseMax[interval]), move(PromiseMin[interval]))); //передаем ссылку
    }

    //запуск потоков
    for (int i = 0; i < n; i++)
    {
        t[i].detach();
    }

    //для каждого из интервалов
    for (int interval = 0; interval < n; interval++)
    {
        //из каждого футуре получаем результат
        int MaxIntervalNumber = FutureMax[interval].get();
        int MinIntervalNumber = FutureMin[interval].get();

        //сравниваем их
        if (MaxIntervalNumber > MaxNumber)
        {
            MaxNumber = MaxIntervalNumber;
        }
        if (MinIntervalNumber < MinNumber)
        {
            MinNumber = MinIntervalNumber;
        }
    }

    //считаем время
    End = std::chrono::high_resolution_clock::now();
    WorkingTime = End - Begin;
    float Time = WorkingTime.count();

    //выводим
    cout << n << ": " << Time << "s" << endl;
}

int main() {
    //инициализация
    int Length;
    cin >> Length;

    srand(time(NULL));

    vector<int> NumbersVector;

    //заполняем вектор
    for (int i = 0; i < Length; i++)
    {
        NumbersVector.push_back(rand());
    }

    //для двух потоков
    FindMinMaxOnInterval(2, NumbersVector);
    //для четырех
    FindMinMaxOnInterval(4, NumbersVector);
    //для восьми
    FindMinMaxOnInterval(8, NumbersVector);
    //для шестнадцати
    FindMinMaxOnInterval(16, NumbersVector);

    return 0;
}


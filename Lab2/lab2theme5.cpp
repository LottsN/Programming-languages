#include <iostream>
#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include "UserInterruptHandler.h"

using namespace std;

int l, n, m;

//структура устройства
struct Device
{
    int number;
    int group;
    int restWorkTime = -1;
    Device(int number, int group) : number(number), group(group) {}
};

//структура приложения
struct App
{
    int group;
    int priority;
    int workingTime;
};

//сравнение для очереди с приоритетом
struct Compare {
    bool operator()(App const& p1, App const& p2)
    {
        return p1.priority < p2.priority;
    }
};

//класс суперочереди
class SuperQueue
{
public:
    //возвращает размер
    size_t getSize() const {
        size_t sum = 0;
        for (const auto &queue : Queues) {
            sum += queue.size();
        }
        return sum;
    }

    //проверяет на пустоту
    bool isEmpty(int group) const {
        return Queues[group].empty();
    }

    //создаем нужное количество очередей
    explicit SuperQueue(int group_num) {
        Queues.resize(group_num);
    }

    //возвращет вверхний элемент
    App giveTop(int group) const {
        return Queues[group].top();
    }

    void printQueue() {
        priority_queue<App, vector<App>, Compare> printing_queue;
        for (const auto &Queue : Queues)
        {
            CopyQueue(printing_queue, Queue);
        }

        while (!printing_queue.empty()) {
            cout << "G" << printing_queue.top().group + 1 << "P" << printing_queue.top().priority + 1 << ", ";
            printing_queue.pop();
        }
    }

    //возвращает элемент на вершине и удаляет
    void popIt(int group) {
        Queues[group].pop();
    }

    //добавляет
    void pushApp(const App &value) {
        Queues[value.group].push(value);
    }



private:
    //копирует для вывода
    void CopyQueue(priority_queue<App, vector<App>, Compare>& new_array, priority_queue<App, vector<App>, Compare> source_array)
    {
        while (!source_array.empty())
        {
            new_array.push(source_array.top());
            source_array.pop();
        }
    }

    //вектор очередей
    vector<priority_queue<App, vector<App>, Compare>> Queues;


};

//прерывание
atomic_bool interruption(false);

//функция генератора
void AppGeneratorFunction(SuperQueue &superqueue, mutex& mtx)
{
    while (true) {
        if (interruption.load())
        {
            cout << "App generator end work\n";
            break;
        }
        int t = rand() % 2 + 1;
        this_thread::sleep_for(chrono::seconds(t));
        unique_lock<mutex> lck(mtx); //приостанавливаем поток пока не получим оповещение
        if (superqueue.getSize() < l) //если есть места - добавляем приложения
        {
            App Application{ rand()%n, rand()%3, 10 + rand()%4 };
            superqueue.pushApp(Application);
        }
    }
}

void DeviceFunction(SuperQueue &superqueue, Device &device, mutex &mtx)
{
    while (true)
    {
        if (interruption.load()) {
            cout << "Device" << device.number + 1 << " ended work\n";
            break;
        }
        unique_lock<mutex> lck(mtx); //не может работать пока идёт вывод
        if (!superqueue.isEmpty(device.group)) //если группу устройства не пуста
        {
            App Application = superqueue.giveTop(device.group); //берем приложение из этой группы
            superqueue.popIt(device.group); //удаляем приложение из очереди
            lck.unlock();
            for (device.restWorkTime = Application.workingTime; device.restWorkTime > 0; device.restWorkTime--) //выполняем приложение
            {
                this_thread::sleep_for(chrono::milliseconds (1000));
            }
            device.restWorkTime = -1;
        }
        else //если очередь пуста - ее можно модифицировать
        {
            lck.unlock();
            this_thread::sleep_for(chrono::milliseconds (200));
        }
    }
}

int main()
{
    cin >> l >> n >> m;
    //л размер очереди
    //н групп
    //м сколько приборов в группе

    SuperQueue superqueue(n);
    mutex mtx;
    vector<vector<Device>> Groups;
    vector<thread> ThreadsArray;

    auto AppGenerator = thread(AppGeneratorFunction, ref(superqueue), ref(mtx));
    Groups.reserve(n);
    ThreadsArray.reserve(n * m); //в каждой группе по н*м устройств - всего н*м потоков

    for (int group = 0; group < n; ++group)
    {
        Groups.emplace_back(); //добавляем очереди
        Groups[group].reserve(m); //группа равна количеству приборов

        for (int i = 0; i < m; ++i)
        {
            Groups[group].emplace_back(i, group); //добавляем очередь и группу
            ThreadsArray.emplace_back(thread(DeviceFunction, ref(superqueue), ref(Groups[group][i]), ref(mtx)));
        }
    }

    using namespace chrono_literals;

    UserInterruptHandler intHandler;

    try
    {

        while (true)
        {

            system("cls");

            mtx.lock(); //только один поток может писать

            //вывод
            cout << "Queue: " << superqueue.getSize() << "/" << l << " ";
            superqueue.printQueue();
            cout << "\n";


            //для каждой группы
            for (int group = 0; group < n; ++group)
            {
                cout << "Group " << group + 1 << "\n";
                //для каждого устройства в очереди
                for (int device = 0; device < m; ++device)
                {

                    int remaining = Groups[group][device].restWorkTime;
                    cout << "Device " << device + 1 << "  ";

                    if (remaining < 0)
                    {
                        cout << "free\n";
                    }

                    else
                    {
                        cout << "busy(" << remaining << "s)\n";
                    }

                }
            }

            mtx.unlock(); //когда вывели открывает поток
            //вывели - другие потоки пошли работать

            this_thread::sleep_for(200ms);
            intHandler.checkInterruptionAndThrow();

        }
    }
    catch (const runtime_error& e)
    {
        cout << e.what() << "\n";
        interruption.store(true);
    }

    //завершаем потоки
    AppGenerator.join();
    for (auto& t : ThreadsArray)
    {
        t.join();
    }
    return 0;
}
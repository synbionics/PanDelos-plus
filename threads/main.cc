#include "ThreadPool.hh"
#include <thread>
#include <functional>
#include <chrono>

int main() {
    threads::ThreadPool pool;
    size_t cellNum = 160000;
    size_t arr[cellNum];
    for(size_t i = 0; i <cellNum; ++i)
        arr[i] = 0;

    pool.start();
    for(size_t i = 0; i <cellNum; ++i)
        pool.execute(
            [&arr, i] {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                // std::cerr<<"\n"<<std::this_thread::get_id<<" adding "<<i;
                arr[i] = i;
            }
    );

    while(!pool.tasksCompleted())
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cerr<<"\nwork done";
    pool.stop();
    std::cerr<<"\npool stop";
    // for(int i = 0; i <cellNum; ++i)
    //     std::cerr<<"\ni: "<<arr[i];
    return 0;
}
#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace par{

class Work{
public:
    Work() = default;
    Work(const Work&) = default;
    Work(Work&&) = default;
    Work& operator=(const Work&) = default;
    Work& operator=(Work&&) = default;
    virtual ~Work() = default;

    virtual void call() = 0;
};

class Calculation : public Work{
public:
    Calculation(std::function<void()> func) : _func{func} {}
    Calculation() = default;
    Calculation(const Calculation&) = default;
    Calculation(Calculation&&) = default;
    Calculation& operator=(const Calculation&) = default;
    Calculation& operator=(Calculation&&) = default;
    virtual ~Calculation() = default;

    void call() override{ _func(); }

    std::function<void()> _func;
};

class Flow : public Work{
public:
    Flow() = default;
    Flow(Flow&&) = default;
    Flow& operator=(Flow&&) = default;
    virtual ~Flow() = default;

    void add(Work* work){
        _work.push_back(work);
    }

    void call() override{ 
        for(auto& work : _work){
            work->call();
        }
    }
private:
    std::vector<Work*> _work;

};
}

class Executor{
public:
    Executor() = default;
    Executor(const Executor&) = default;
    Executor(Executor&&) = default;
    Executor& operator=(const Executor&) = default;
    Executor& operator=(Executor&&) = default;
    ~Executor(){
        for(auto& thread : _worker_threads){
            thread.join();
        }
        _main_thread.join();
    }

    Executor(int num_threads)
    {
        async_init(num_threads);
    }

    void run(Work* work)
    {
        std::unique_lock<std::mutex> lock(*_mutex);
        _work.push_back(&work);
    }

private:
    void async_init(size_t num_threads)
    {
        _main_thread = std::thread{[this, num_threads](){
            for(size_t i = 0; i < num_threads; ++i){
                _worker_threads.emplace_back([this](){ execute_worker_thread(); });
            }
        }};
    }

    void execute_worker_thread()
    {
        for(;;){
            auto* work = pop_work();
            if(work == nullptr){
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            if(work){
                work->call();
            }
        }
    }

    Work* pop_work()
    {
        std::unique_lock<std::mutex> lock(*_mutex);
        auto* work = _work.back();
        _work.pop_back();
        return work;
    }

    std::thread _main_thread;
    std::vector<std::thread> _worker_threads;
    std::vector<Work*> _work;
    std::shared_ptr<std::mutex> _mutex = std::make_shared<std::mutex>();

};

}
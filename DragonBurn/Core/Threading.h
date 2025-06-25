#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <memory>
#include <type_traits>

#undef max()
#undef min()

namespace Threading {

    // Thread-safe queue for tasks
    template<typename T>
    class SafeQueue {
    private:
        mutable std::mutex mtx_;
        std::queue<T> queue_;
        std::condition_variable condition_;

    public:
        SafeQueue() = default;
        SafeQueue(const SafeQueue&) = delete;
        SafeQueue& operator=(const SafeQueue&) = delete;

        void push(T item) {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(item);
            condition_.notify_one();
        }

        bool tryPop(T& item) {
            std::lock_guard<std::mutex> lock(mtx_);
            if (queue_.empty()) return false;
            item = queue_.front();
            queue_.pop();
            return true;
        }

        void waitAndPop(T& item) {
            std::unique_lock<std::mutex> lock(mtx_);
            while (queue_.empty()) {
                condition_.wait(lock);
            }
            item = queue_.front();
            queue_.pop();
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return queue_.empty();
        }

        size_t size() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return queue_.size();
        }
    };

    // High-performance thread pool
    class ThreadPool {
    private:
        std::vector<std::thread> workers_;
        SafeQueue<std::function<void()>> tasks_;
        std::atomic<bool> stop_;
        size_t thread_count_;

        void workerThread() {
            while (!stop_) {
                std::function<void()> task;
                tasks_.waitAndPop(task);

                // Check if we got a valid task (stop condition might have been triggered)
                if (task) {
                    task();
                }
            }
        }

    public:
        explicit ThreadPool(size_t threads = std::thread::hardware_concurrency())
            : stop_(false), thread_count_(threads) {
            for (size_t i = 0; i < threads; ++i) {
                workers_.emplace_back([this] { workerThread(); });
            }
        }

        ~ThreadPool() {
            stop_ = true;

            // Wake up all sleeping threads by pushing dummy tasks
            for (size_t i = 0; i < thread_count_; ++i) {
                tasks_.push([]() {});
            }

            for (auto& worker : workers_) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

        template<typename F, typename... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> {
            using return_type = typename std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<return_type> result = task->get_future();

            tasks_.push([task]() { (*task)(); });

            return result;
        }

        void waitForCompletion() {
            while (!tasks_.empty()) {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        }

        size_t getThreadCount() const { return thread_count_; }
    };

    // Parallel for loop execution
    template<typename Iterator, typename Function>
    void parallelFor(Iterator begin, Iterator end, Function func, size_t chunk_size = 0) {
        const size_t total_size = std::distance(begin, end);
        if (total_size == 0) return;

        static ThreadPool pool;
        const size_t num_threads = pool.getThreadCount();

        if (chunk_size == 0) {
            chunk_size = std::max(1ULL, total_size / (num_threads * 4));
        }

        std::vector<std::future<void>> futures;
        futures.reserve((total_size + chunk_size - 1) / chunk_size);

        for (auto it = begin; it < end; it += chunk_size) {
            auto chunk_end = std::min(it + chunk_size, end);

            futures.push_back(pool.enqueue([it, chunk_end, func]() {
                for (auto current = it; current != chunk_end; ++current) {
                    func(*current);
                }
                }));
        }

        // Wait for all tasks to complete
        for (auto& future : futures) {
            future.wait();
        }
    }

    // Parallel for with index
    template<typename Function>
    void parallelForIndex(size_t start, size_t end, Function func, size_t chunk_size = 0) {
        if (start >= end) return;

        static ThreadPool pool;
        const size_t num_threads = pool.getThreadCount();
        const size_t total_size = end - start;

        if (chunk_size == 0) {
            chunk_size = std::max(1ULL, total_size / (num_threads * 4));
        }

        std::vector<std::future<void>> futures;
        futures.reserve((total_size + chunk_size - 1) / chunk_size);

        for (size_t i = start; i < end; i += chunk_size) {
            size_t chunk_end = std::min(i + chunk_size, end);

            futures.push_back(pool.enqueue([i, chunk_end, func]() {
                for (size_t idx = i; idx < chunk_end; ++idx) {
                    func(idx);
                }
                }));
        }

        // Wait for all tasks to complete
        for (auto& future : futures) {
            future.wait();
        }
    }

    // Batch processor for collecting results
    template<typename T, typename Function>
    auto parallelMap(const std::vector<T>& input, Function func) -> std::vector<decltype(func(input[0]))> {
        using ResultType = decltype(func(input[0]));
        std::vector<ResultType> results(input.size());

        parallelForIndex(0, input.size(), [&](size_t i) {
            results[i] = func(input[i]);
            });

        return results;
    }

    // Thread-safe data collector
    template<typename T>
    class ThreadSafeVector {
    private:
        std::vector<T> data_;
        mutable std::mutex mtx_;

    public:
        void push_back(const T& item) {
            std::lock_guard<std::mutex> lock(mtx_);
            data_.push_back(item);
        }

        void push_back(T&& item) {
            std::lock_guard<std::mutex> lock(mtx_);
            data_.push_back(std::move(item));
        }

        std::vector<T> get() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return data_;
        }

        void clear() {
            std::lock_guard<std::mutex> lock(mtx_);
            data_.clear();
        }

        size_t size() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return data_.size();
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return data_.empty();
        }
    };

    // Get singleton thread pool instance
    inline ThreadPool& getGlobalThreadPool() {
        static ThreadPool pool;
        return pool;
    }
}
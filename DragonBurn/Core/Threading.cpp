#include "Threading.h"
#include <algorithm>
#include <chrono>

namespace Threading {

    // Additional utility functions for the threading library

    size_t getOptimalThreadCount() {
        const size_t hardware_threads = std::thread::hardware_concurrency();
        return hardware_threads > 0 ? hardware_threads : 4;
    }

    size_t calculateOptimalChunkSize(size_t total_items, size_t thread_count) {
        if (total_items == 0 || thread_count == 0) return 1;

        // Aim for 4x more chunks than threads for better load balancing
        const size_t target_chunks = thread_count * 4;
        const size_t chunk_size = std::max(1ULL, total_items / target_chunks);

        return chunk_size;
    }

    // Specialized parallel executor for game loops
    template<typename EntityContainer, typename ProcessFunc>
    void parallelEntityProcessor(EntityContainer& entities, ProcessFunc processFunc, size_t chunk_size = 0) {
        if (entities.empty()) return;

        static ThreadPool& pool = getGlobalThreadPool();
        const size_t num_threads = pool.getThreadCount();
        const size_t total_entities = entities.size();

        if (chunk_size == 0) {
            chunk_size = calculateOptimalChunkSize(total_entities, num_threads);
        }

        std::vector<std::future<void>> futures;
        futures.reserve((total_entities + chunk_size - 1) / chunk_size);

        for (size_t i = 0; i < total_entities; i += chunk_size) {
            size_t end_idx = std::min(i + chunk_size, total_entities);

            futures.push_back(pool.enqueue([&entities, processFunc, i, end_idx]() {
                for (size_t idx = i; idx < end_idx; ++idx) {
                    processFunc(entities[idx], idx);
                }
                }));
        }

        // Wait for all processing to complete
        for (auto& future : futures) {
            future.wait();
        }
    }

    // Performance monitoring utilities
    class PerformanceTimer {
    private:
        std::chrono::high_resolution_clock::time_point start_time_;

    public:
        PerformanceTimer() : start_time_(std::chrono::high_resolution_clock::now()) {}

        double elapsedMs() const {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
            return duration.count() / 1000.0;
        }

        void reset() {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
    };

} // namespace Threading
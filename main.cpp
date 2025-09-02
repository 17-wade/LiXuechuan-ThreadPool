#include <iostream>
#include <vector>
#include <chrono>
#include <future>
#include "thread_pool.h"

// Example task function
void example_task(int id, int duration) {
    std::cout << "Task " << id << " started on thread " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(duration));
    std::cout << "Task " << id << " completed on thread " << std::this_thread::get_id() << std::endl;
}

// Function that returns a value
int compute_square(int num) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return num * num;
}

int main() {
    // Create a thread pool with 4 threads
    ThreadPool pool(4);

    // Example 1: Submit tasks without return values
    std::cout << "=== Example 1: Basic Tasks ===" << std::endl;
    auto task1 = pool.enqueue(example_task, 1, 2);
    auto task2 = pool.enqueue(example_task, 2, 1);
    auto task3 = pool.enqueue(example_task, 3, 3);

    // Wait for these tasks to complete
    task1.wait();
    task2.wait();
    task3.wait();

    // Example 2: Submit tasks with return values
    std::cout << "\n=== Example 2: Tasks with Return Values ===" << std::endl;
    std::vector<std::future<int>> futures;
    
    for (int i = 1; i <= 8; ++i) {
        futures.emplace_back(pool.enqueue(compute_square, i));
    }

    // Collect results
    for (int i = 0; i < 8; ++i) {
        std::cout << "Square of " << (i+1) << " is " << futures[i].get() << std::endl;
    }

    // Example 3: Parallel computation
    std::cout << "\n=== Example 3: Parallel Computation ===" << std::endl;
    const int N = 1000000;
    std::vector<int> data(N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        data[i] = i % 100;
    }

    // Process data in parallel
    const int num_tasks = 4;
    const int chunk_size = N / num_tasks;
    std::vector<std::future<long long>> results;
    
    auto sum_chunk = [](const std::vector<int>& data, int start, int end) -> long long {
        long long sum = 0;
        for (int i = start; i < end; ++i) {
            sum += data[i];
        }
        return sum;
    };

    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_tasks; ++i) {
        int start = i * chunk_size;
        int end = (i == num_tasks - 1) ? N : (i + 1) * chunk_size;
        results.emplace_back(pool.enqueue(sum_chunk, std::cref(data), start, end));
    }

    // Collect and sum results
    long long total_sum = 0;
    for (auto& result : results) {
        total_sum += result.get();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Parallel sum: " << total_sum << std::endl;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    // For comparison, do it sequentially
    start_time = std::chrono::high_resolution_clock::now();
    long long sequential_sum = 0;
    for (int i = 0; i < N; ++i) {
        sequential_sum += data[i];
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Sequential sum: " << sequential_sum << std::endl;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    return 0;
}
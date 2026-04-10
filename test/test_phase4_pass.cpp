#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <pthread.h>
struct lockfree_data {
    std::vector<float> trajectories[3];
    std::atomic<int> newest_ready_idx{0};
};
void function1(lockfree_data &data) {
    int currentwinx = 1;
    int nextwinx = 2;
    float framecounter = 0.0f;
    
    while(true) {
        framecounter += 1.0f;
        data.trajectories[currentwinx].assign(50, framecounter);
        std::this_thread::sleep_for(std::chrono::milliseconds(35));
        nextwinx = data.newest_ready_idx.exchange(currentwinx);
        currentwinx = nextwinx;
    }   
}
void function2(lockfree_data& data) {
    int current_read_idx = 0;
    const auto period = std::chrono::microseconds(2000); 
    const auto failure_threshold = std::chrono::microseconds(2500);
    auto target_time = std::chrono::high_resolution_clock::now() + period;

    while(true) {
        auto start_time = std::chrono::high_resolution_clock::now();
        current_read_idx = data.newest_ready_idx.exchange(current_read_idx);
        float current_trajectory_val = 0;
        if (!data.trajectories[current_read_idx].empty()) {
            current_trajectory_val = data.trajectories[current_read_idx][0];
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto actual_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        if (actual_duration > failure_threshold) {
            std::cout << actual_duration.count() << " us\n";
        }
        
        std::this_thread::sleep_until(target_time);
        target_time += period;
    }
}

int main() {
    lockfree_data shared_data;
    std::thread ai_thread(function1, std::ref(shared_data));
    std::thread robot_thread(function2, std::ref(shared_data));
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    if (pthread_setaffinity_np(robot_thread.native_handle(), sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "failed to pin robot to core 1\n";
    }
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);
    if (pthread_setaffinity_np(ai_thread.native_handle(), sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "failed to pin inference to core 2\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ai_thread.detach();
    robot_thread.detach();

    return 0;
}

#pragma once
#include<atomic>
#include<vector>
#include<chrono>

struct sysHealth{
    std::atomic<long long> lidar_timestamp{0};
    std::atomic<long long> offset{0};
    std::atomic<bool> stoptrigger{false};
};
class fsyncController{
private:
    const long long nominal_interval_ns= 33333333;
    const long long max_slew_ns = 50000;

    double Kp =0.2;
    double Ki =0.01;

    long long integralAccum = 0;

public:
    long long calculate_next_interval(long long current_camera_ts, long long current_lidar_ts);
    void run_camera_thread(sysHealth& health, std::vector<long long>& telemetry_buffer);
};
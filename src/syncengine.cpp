    #include<atomic>
    #include<vector>
    #include<chrono>
    #include<algorithm>
    #include "syncengine.h" 
    #include <thread>

    long long fsyncController::calculate_next_interval(long long current_camera_ts, long long current_lidar_ts){
        long long delta = current_camera_ts-current_lidar_ts;
        double pterm = delta*Kp; 
        integralAccum+=delta;
        double iterm=Ki*integralAccum;
        long long rawadj = static_cast<long long>(pterm+iterm); 
        
        long long clamp = std::clamp(rawadj,-max_slew_ns, max_slew_ns);
        
        return nominal_interval_ns-clamp;
    }

    void fsyncController::run_camera_thread(sysHealth& health, std::vector<long long>& telemetry_buffer){
        using clock = std::chrono::high_resolution_clock;
        while(!health.stoptrigger){
            long long camera_ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
                clock::now().time_since_epoch()
            ).count();
            long long lidar_ts = health.lidar_timestamp;
            if (lidar_ts == 0) {
            std::this_thread::yield(); 
            continue;
        }

            long long delta = camera_ts-lidar_ts;
            if(delta>5000000 || delta<-5000000){
                health.stoptrigger.store(true);
                break;
            }
            long long interval = calculate_next_interval(camera_ts, lidar_ts);
            telemetry_buffer.push_back(delta);

            std::this_thread::sleep_for(std::chrono::nanoseconds(interval));
        }
    }
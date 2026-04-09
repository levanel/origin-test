#include "syncengine.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>

//lets mock a lidar
void pseudolidar(sysHealth& health){
    while(!health.stoptrigger.load()){
        auto now  = std::chrono::high_resolution_clock::now();
        health.lidar_timestamp.store(now.time_since_epoch().count());

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

int main(){
    sysHealth health;
    fsyncController controller;
    std::vector<long long> telemetry_buffer;

    telemetry_buffer.reserve(1000);//prealloc
    std::thread lidar(pseudolidar, std::ref(health));

    std::thread camera(&fsyncController::run_camera_thread, &controller, std::ref(health), std::ref(telemetry_buffer));

    std::this_thread::sleep_for(std::chrono::seconds(10));
    health.stoptrigger.store(true);

    lidar.join();
    camera.join();
    
    std::cout << "\n runnin ts (pun intended).. \n";
    std::cout << " \n output :\n";
    //jus put them numbers in the terminal lil bro
    size_t start_idx = (telemetry_buffer.size() > 20) ? telemetry_buffer.size() - 20 : 0;
    for (size_t i = start_idx; i < telemetry_buffer.size(); ++i) {
        std::cout<<"Frame "<< i << " offset: "<<telemetry_buffer[i]<<" ns\n";
    }
return 0;
}
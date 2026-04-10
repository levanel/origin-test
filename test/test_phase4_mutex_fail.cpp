    #include<iostream>
    #include<mutex>
    #include<vector>
    #include<thread>
    struct dummyval{
        std::mutex mtx;
        std::vector<float> trajectory;
        int framecounter;
    };

    void function1(dummyval &data){
    while(true){
        data.trajectory.resize(50);
        data.mtx.lock();
        for(int i = 0; i<50; i++){
            data.trajectory[i] = i;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(35));
        data.mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
    }

    void function2(dummyval &data){
        const auto period = std::chrono::microseconds(2000);
        const auto failure = std::chrono::microseconds(2500);
        auto target = std::chrono::high_resolution_clock::now()+period; 
        while(true){
            auto startime =std::chrono::high_resolution_clock::now();
            data.mtx.lock();
            int current_frame = data.framecounter;
            data.mtx.unlock();

            auto endtime = std::chrono::high_resolution_clock::now();
            auto actual_duration = std::chrono::duration_cast<std::chrono::microseconds>(endtime - startime);

            if(actual_duration>failure){
                std::cout<<actual_duration.count()<<" us"<<"\n";
            }
            std::this_thread::sleep_until(target);
            target+=period;
        }
    }
int main() {
    dummyval shared_data;
    shared_data.framecounter = 0;
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

#pragma once
#include <dme/core/utils.h>
namespace dme {
    //测试耗时
    class TestTimer {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> create;
        bool output;
    public:
        TestTimer() noexcept : create(std::chrono::high_resolution_clock::now()), output(true) {}
        ~TestTimer() {
            stop();
        }
        void closeOutput() noexcept {
            output = false;
        }
        void stop() const noexcept {
            if (!output) return;
            auto now = std::chrono::high_resolution_clock::now();
            //auto start = std::chrono::time_point_cast<std::chrono::microseconds>(create).time_since_epoch().count();
            //auto end = std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count();
            auto ns = (now - create).count();
            slong fps = 100000000000 / ns;

            std::cout << "TestTimer: " << ns << "ns (" << fps << "fps)\n";
        }
    };

}
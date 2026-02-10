#pragma once

#include <iostream>
#include <chrono>

class Stopwatch {
public:
    Stopwatch() : start_time(), end_time(), running(false) {}

    void start() {
        if (!running) {
            start_time = std::chrono::high_resolution_clock::now();
            running = true;
        }
    }

    void stop() {
        if (running) {
            end_time = std::chrono::high_resolution_clock::now();
            running = false;
        }
    }

    void reset() {
        running = false;
    }

    double elapsed() {
        if (running) {
            end_time = std::chrono::high_resolution_clock::now();
        }
        return std::chrono::duration<double>(end_time - start_time).count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool running;
};

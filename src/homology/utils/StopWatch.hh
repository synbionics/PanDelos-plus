#include <chrono>


#ifndef STOP_WATCH_INCLUDE_GUARD
#define STOP_WATCH_INCLUDE_GUARD

namespace stopwatch {
    

    class StopWatch {
        private:
            using timePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
            timePoint startTime;

        public:
            StopWatch(/* args */);
            ~StopWatch();
            void start();
            unsigned int stop(char c) const;
    };
    
    StopWatch::StopWatch() {
    
    }
    void StopWatch::start(){
        startTime = std::chrono::high_resolution_clock::now();
    }

    // n = nanoseconds, s = seconds, default:milliseconds
    unsigned int StopWatch::stop(char c) const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - startTime;
        if(c == 'n')
            return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        if(c == 's')
            return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
    
    StopWatch::~StopWatch() {
    }

}



#endif
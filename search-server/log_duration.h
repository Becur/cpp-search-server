#pragma once

#include <chrono>
#include <ostream>
#include <string>
#include <iostream>

#define PROFILE_CONCAT_INTERNAL(X, Y) X ## Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x) 
#define LOG_DURATION_STREAM(x, y) LogDuration UNIQUE_VAR_NAME_PROFILE(x, y) 

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(const std::string& name_func, std::ostream& out = std::cerr)
    : name(name_func)
    , flow(out){
    }

    ~LogDuration(){
    using namespace std::chrono;
    using namespace std::literals;

    const auto end_time = Clock::now();
    const auto dur = end_time - start_time_;
    if(!name.empty()){
        flow << name;
    }
    flow << ": " << duration_cast<milliseconds>(dur).count() << " ms\n";
    }

private:
    std::string name;
    std::ostream& flow;
    const Clock::time_point start_time_ = Clock::now();
};
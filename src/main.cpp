#include "cms/cms_utils.hpp"
#include "spdlog/logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <atomic>
#include <chrono>
#include <csetjmp>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>
#include <filesystem>
class Manager : public std::enable_shared_from_this<Manager>{
private:
    std::string taskName_;
    std::atomic<int> time_{0};
    std::atomic<bool> stopFlag_{false};
    std::atomic<bool> pauseFlag_{false};
    std::shared_ptr<spdlog::logger> logger_;

public:
    Manager(std::shared_ptr<spdlog::logger> logger) : logger_(logger) {
        logger_->info("Manager built");
    }
    
    Manager& SetTaskName(const std::string& taskname) {taskName_ = taskname;logger_->info("Task name set to: {}", taskname);return *this;}
    Manager& SetTime(const int& time){time_.store(time);logger_->info("Task time set to: {}",time_.load());return *this;}

    std::string GetTaskName(){return taskName_;}
    int GetTime(){return time_.load();}

    void StartTask() {
    cms::ClearScreen();
    
    std::cout << "\n\n\n";  
    std::cout << "> " << std::flush; 
    std::cout << "\033[s"; 
    
    while (!stopFlag_.load()) {
        std::cout << "\033[u";  
        printf("\r%-40s\r", "");
	std::cout << "==========" << GetTaskName() << "==========" << std::endl;
        printf("Time : %02d:%02d  State: %s", 
               time_.load()/60, time_.load()%60,
               pauseFlag_.load() ? "PAUSED" : "RUNNING");
        std::cout << std::flush;
        std::cout << "\033[2B";
        std::cout << "\r> " << std::flush;
        if(time_.load()==0){break;}
        if (!pauseFlag_.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            time_.fetch_sub(1);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
	
    }
}
    void HandleQuit(){
	    std::weak_ptr<Manager> wekptr = shared_from_this();
	    std::string cms;
	    while (!(stopFlag_.load()) && time_.load()>0) {
	    std::cout<<'\n' << std ::flush;
	    auto sharedThis = wekptr.lock();if (!sharedThis){return;}
	    cms=cms::SafeInput<std::string>("");
	    sharedThis=wekptr.lock();if(!sharedThis){return;}
	    if (cms=="q"){sharedThis->logger_->info("quit");sharedThis->stopFlag_ .store(true);break;}
	    
	    if (cms=="p"){sharedThis->logger_->info("pause");cms::ClearScreen();sharedThis->pauseFlag_.store(true);}
	    if (cms=="r"){sharedThis->logger_->info("resume");cms::ClearScreen();sharedThis->pauseFlag_.store(false);}
	    }
	    if (auto sharedThis = wekptr.lock()) {
	    	sharedThis->logger_->info("input exiting");
	    }
    }


};
   
int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "How to use " << argv[0] << " <taskname> <time(minutes)>" << std::endl;
        std::cout << "Time supports decimals, for example: 1.5 represents 1 minute and 30 seconds" << std::endl;
        return 1;
    }

    
    std::filesystem::create_directories("logs");

    
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/idk.log", true);
    auto logger = std::make_shared<spdlog::logger>("Main", fileSink);

    cms::ClearScreen();
    logger->info("Arguments:");
    for (int i = 0; i < argc; ++i) {logger->info("  argv[{}] = {}", i, argv[i]);}

    
    auto manager= std::make_shared<Manager>(logger);
    try {double minutes = std::stod(argv[2]);  int seconds = static_cast<int>(minutes * 60);  manager->SetTaskName(argv[1]).SetTime(seconds);
    } catch (...) {
        std::cout << "Time format error" << std::endl;
        return 1;
    }

    
    std::thread quit([manager]{manager->HandleQuit();});quit.detach();  
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    
    manager->StartTask();


    
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    cms::ClearScreen();
    std::string msg = "Time's up\nAnd have a nice day bruh!\nCodeManStudio Finished";
    for (char e : msg) {
        std::cout << e << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << std::endl;

    return 0;
}

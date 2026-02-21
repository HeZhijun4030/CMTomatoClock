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
#include "cms/cms_utils.hpp"
class Manager : public std::enable_shared_from_this<Manager>{
private:
    enum Phase {WORK,BREAK};
    Phase curPhase_ = Phase::WORK;
    std::string taskName_;
    std::atomic<int> time_{0};
    std::atomic<bool> stopFlag_{false};
    std::atomic<bool> pauseFlag_{false};
    std::shared_ptr<spdlog::logger> logger_;

    int tomatoCount_=0;
    int targetCount_=0;

    int workTime_ = 25*60;
    int breakTime_= 15*60;


public:
    Manager(std::shared_ptr<spdlog::logger> logger,const int &tcount)
            : logger_(logger),
              targetCount_(tcount)
        {
        logger_->info("Manager built");
    }
    Manager(std::shared_ptr<spdlog::logger> logger,const int &tcount,const int& workTime,const int& breakTime)
            : logger_(logger),
            targetCount_(tcount),
            workTime_(workTime),
            breakTime_(breakTime)
    {
            logger_->info("Manager built");
    }


    Manager& SetTaskName(const std::string& taskname) {taskName_ = taskname;logger_->info("Task name set to: {}", taskname);return *this;}
    Manager& SetTime(const int& wtime,const int& btime){breakTime_=btime;workTime_=wtime;logger_->info("Task time set to: {}",time_.load());return *this;}




    std::string GetTaskName(){return taskName_;}
    int GetTime(){return time_.load();}
    void DoCountDown(){
        while(!(stopFlag_.load()) && time_.load()>0){
        std::cout << "\033[2J\033[H";
        printf("\r%-40s\r", "");
        switch (curPhase_) {
                case Phase::WORK:
                        std::cout << "==========" << GetTaskName() << "==========" << std::endl;
                        break;
                case Phase::BREAK:
                        std::cout << "==========Breaking==========" << std::endl;
                        break;
        }
        printf("Time : %02d:%02d  State: %s",
        time_.load()/60, time_.load()%60,
        pauseFlag_.load() ? "PAUSED" : "RUNNING");
        std::cout << std::flush;
        std::cout << "\033[2B";
        std::cout << "\r> " << std::flush;
        if(time_.load()<0){break;}
        if (!pauseFlag_.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            time_.fetch_sub(1);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

    }


    }
    void StartTask() {
    cms::ClearScreen();

    std::cout << "\n\n\n";
    std::cout << "> " << std::flush;
    std::cout << "\033[s";

    while (!stopFlag_.load() && tomatoCount_ < targetCount_) {
            curPhase_=Phase::WORK;
            time_.store(workTime_);
            DoCountDown();
            if (stopFlag_.load() ){return;}
            ++tomatoCount_;

            cms::ClearScreen();
            curPhase_=Phase::BREAK;
            time_.store(breakTime_);
            DoCountDown();
            if (tomatoCount_ >= targetCount_){break;}
    }
}
    void HandleQuit(){
            std::weak_ptr<Manager> wekptr = shared_from_this();
            std::string cms;
            while (!(stopFlag_.load())) {
            std::cout<<'\n' << std ::flush;
            auto sharedThis = wekptr.lock();if (!sharedThis){return;}
            cms=cms::SafeInput<std::string>("");
            sharedThis=wekptr.lock();if(!sharedThis){return;}
            if (cms=="q"){sharedThis->logger_->info("quit");sharedThis->stopFlag_ .store(true);break;}

            if (cms=="p"){sharedThis->logger_->info("pause");cms::ClearScreen();sharedThis->pauseFlag_.store(true);}
            if (cms=="r"){sharedThis->logger_->info("resume");cms::ClearScreen();sharedThis->pauseFlag_.store(false);}
            if (cms=="s"){sharedThis->time_.store(0);}
            }

            if (auto sharedThis = wekptr.lock()) {
                sharedThis->logger_->info("input exiting");
            }
    }


};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "How to use " << argv[0] << " <taskname> <tomatoCount>\nor <taskname> <tomatoCount> <workTime> <breaktime>" << std::endl;
        std::cout << "Time supports decimals, for example: 1.5 represents 1 minute and 30 seconds" << std::endl;
        return 1;
    }


    std::filesystem::create_directories("logs");


    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/idk.log", true);
    auto logger = std::make_shared<spdlog::logger>("Main", fileSink);

    cms::ClearScreen();
    logger->info("Arguments:");
    for (int i = 0; i < argc; ++i) {logger->info("  argv[{}] = {}", i, argv[i]);}



    std::shared_ptr<Manager> manager;

   if (argc==3){
           try {
           manager=std::make_shared<Manager>(logger,std::stoi(argv[2]));
           manager->SetTaskName(argv[1]);
           } catch (...) {
           std::cout<< "unknow error" << std::endl;
           return 1;
           }}
   if (argc==5){
           try {
           manager=std::make_shared<Manager>(logger,std::stoi(argv[2]),std::stod(argv[3])*60,std::stod(argv[4])*60);
           manager->SetTaskName(argv[1]);
           } catch (...) {
                   std::cout<<"unknow error" <<std::endl;
                   return 1;
           }}


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


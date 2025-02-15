#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "util.h"
#include "ProcessParser.h"


using namespace std;

int main() {
    for(auto & pid : ProcessParser::getPidList()) {
        cout << stof(ProcessParser::getVmSize(pid)) * 1024 * 1024 << endl;
        cout << ProcessParser::getProcUpTime(pid) <<endl;
        cout<< ProcessParser::getCmd(pid) <<endl;
        cout << ProcessParser::getProcUser(pid) <<endl;
        cout << ProcessParser::getCpuPercent(pid) <<endl;
    }
    string pid = ProcessParser::getPidList()[5];
    cout << ProcessParser::getSysUpTime() <<endl;
    
    cout<<endl;
    for (auto & item : ProcessParser::getPidList()) {
        cout << item <<endl;
    }
    cout<<endl;
   cout<< ProcessParser::getCmd(pid) <<endl;
   cout << ProcessParser::getNumberOfCores() <<endl;
   
    for (auto & item : ProcessParser::getSysCpuPercent()) {
        cout << item <<endl;
    }

    cout<<"sys ram percent test" <<endl;
    cout<<ProcessParser::getSysRamPercent()<<endl;
    cout<<ProcessParser::getSysKernelVersion()<<endl;
    cout<<ProcessParser::getOSName()<<endl;
    cout<< "thread count: " << ProcessParser::getTotalThreads()<<endl;
    cout<< "total proc: " << ProcessParser::getTotalNumberOfProcesses() <<endl;
    cout<< "running proc: " << ProcessParser::getNumberOfRunningProcesses() <<endl;
    cout<<"false: " << false << ", does pid exist?: " << ProcessParser::isPidExisting(pid) <<endl;
}


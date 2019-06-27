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
    string pid = ProcessParser::getPidList()[5];
    cout << stof(ProcessParser::getVmSize(pid)) * 1024 * 1024 << endl;
    cout << ProcessParser::getProcUpTime(pid) <<endl;
    cout << ProcessParser::getSysUpTime() <<endl;
    cout << ProcessParser::getProcUser(pid) <<endl;
    cout << ProcessParser::getCpuPercent(pid) <<endl;
    /*
    for (auto & item : ProcessParser::getPidList()) {
        cout << item <<endl;
    }
    */
   cout<< ProcessParser::getCmd(pid) <<endl;
   cout << ProcessParser::getNumberOfCores() <<endl;
   
    for (auto & item : ProcessParser::getSysCpuPercent()) {
        cout << item <<endl;
    }

    cout<<"sys ram percent test" <<endl;
    cout<<ProcessParser::getSysRamPercent()<<endl;
    cout<<ProcessParser::getSysKernelVersion()<<endl;
    cout<<ProcessParser::getOSName()<<endl;
}


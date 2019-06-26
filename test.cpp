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
    string pid = "569";
    cout << stof(ProcessParser::getVmSize(pid)) * 1024 * 1024 << endl;
    cout << ProcessParser::getProcUpTime(pid) <<endl;
    cout << ProcessParser::getSysUpTime() <<endl;
    cout << ProcessParser::getProcUser(pid) <<endl;
    cout << ProcessParser::getCpuPercent(pid) <<endl;
}


#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList(); //check
    static std::string getVmSize(string pid); //check
    static std::string getCpuPercent(string pid); //check
    static long int getSysUpTime(); //check
    static std::string getProcUpTime(string pid); //check
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:

string ProcessParser::getVmSize(string pid) {
  string line;
  string name = "VmData";
  string value;
  float result;
  
  //Opening stream for specific file
  ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
  while(std::getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      
      result = (stof(values[1])/float(1024*1024));
      break;
    }
  }
  return to_string(result);
}

string ProcessParser::getProcUpTime(string pid) {
  string line;
  float result;
  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream,line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  return to_string(float(stof(values[13])/sysconf(_SC_CLK_TCK)));
}

long int ProcessParser::getSysUpTime() {
  string line;
  long int result;
  ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  result = stoi(values[0]);
  return result;
}

string ProcessParser::getProcUser(string pid) {
  string line;
  string result;
  string uid;

  ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
  string name = "Uid";

  while(std::getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      
      uid = values[1];
      break;
    }
  }

  stream = Util::getStream("/etc/passwd");
  while(std::getline(stream, line)) {
    stringstream buf(line);
    string word;
    vector<string> parts;
    while(std::getline(buf, word, ':')) {
      parts.push_back(word);
      if(word == uid) {
        result = parts[0];
        return result;
      }
    }
  }

}

std::string ProcessParser::getCpuPercent(string pid) {
  string line;
  string value;
  float result;

  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);

  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  float procUpTime = stof(getProcUpTime(pid));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = getSysUpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = procUpTime + stime + cutime + cstime;
  float seconds = uptime - (starttime/freq);
  result = 100.0*((total_time/freq)/seconds);
  return to_string(result);

}

vector<string> ProcessParser::getPidList() {
  vector<string> result;
  bool isNumber = true;
  DIR *dir;
  struct dirent *ent;
  if((dir = opendir("/proc")) != NULL) {
    while((ent = readdir(dir)) != NULL) {
      for(int i = 0; ent->d_name[i] != '\0'; i++) {
        if(!isdigit(ent->d_name[i])) isNumber = false;
      }
      if(isNumber) result.push_back(ent->d_name);
      isNumber = true;
    }
  }
  closedir(dir);
  return result;
}

string ProcessParser::getCmd(string pid) {
  string line;
  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::cmdPath()));
  getline(stream, line);
  return line;
}
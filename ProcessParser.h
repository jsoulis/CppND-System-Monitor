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
    static string getCmd(string pid); //check
    static vector<string> getPidList(); //check
    static std::string getVmSize(string pid); //check
    static std::string getCpuPercent(string pid); //check
    static long int getSysUpTime(); //check
    static std::string getProcUpTime(string pid); //check
    static string getProcUser(string pid); //check
    static vector<string> getSysCpuPercent(string coreNumber = ""); //check
    static float getSysRamPercent(); //check
    static string getSysKernelVersion(); //check
    static int getNumberOfCores(); //check
    static int getTotalThreads(); //check
    static int getTotalNumberOfProcesses(); //check
    static int getNumberOfRunningProcesses(); //check
    static string getOSName(); //check
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2); //check
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:

string ProcessParser::getVmSize(string pid) {
  string line;
  string name = "VmData";
  string value;
  float result;
  
  //cout<<(Path::basePath() + pid + Path::statusPath())<<endl;
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

  //cout<<(Path::basePath() + pid + "/" + Path::statPath())<<endl;
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

  //cout<<(Path::basePath() + pid + Path::statusPath())<<endl;
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

  //cout<<(Path::basePath() + pid + "/" + Path::statPath())<<endl;
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

  //cout<<(Path::basePath() + pid + Path::cmdPath())<<endl;
  ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
  getline(stream, line);
  return line;
}

int ProcessParser::getNumberOfCores() {
  /*
  this is how we were instructed to implement the getNumberOfCores() function, but I believe it is incorrect.
  In SysInfo.h when the SysInfo constructor is called, the getOtherCores member is set by calling ProcessParser::getNumberOfCores(). This resulted in 8 cores, 2 for 
  each cpu on the system. However, in the SysInfo constructor setAttributes() is also called. If we look at this function, it succeeds until 
  setCPUCoresStats() function is called. In this function, We iteratively call ProcessParser::getSysCpuPercent, passing the core number,  which reads from /proc/stat..
  however, this file only has information about 4 cpus, not 8 cores. Therefore in SysInfo::setCPUCoreStats when building the currentCpuCoresStats vector, we are adding empty 
  vectors that cause the subsequent loop with calls to ProcessParser::PrintCpuStats to crash... Therefore I have changed the implementation of this function
  to return the number of cpus 
  */
  /* ifstream stream = Util::getStream((Path::basePath() + "cpuinfo"));
  while(getline(stream,line)){
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      
      result += stoi(values[3]);
    }
  }
 */
  //TODO:
  int result = 0;
  string name = "cpu cores";
  string line;
  ifstream stream = Util::getStream((Path::basePath() + "cpuinfo"));
  while(getline(stream,line)){
    if(line.compare(0, name.size(), name) == 0) {
      result++;
    }
  }
  //return result;
  return result;

}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber) {
  vector<string> result;
  string line;
  string name = "cpu" + coreNumber;
  ifstream stream = Util::getStream((Path::basePath() + "/" + Path::statPath()));
  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      result = vector<string>(beg, end);
      return result;
    }
  }
}

float getSysActiveCpuTime(vector<string> values) {
  return (stof(values[S_USER]) + 
          stof(values[S_NICE]) + 
          stof(values[S_SYSTEM]) + 
          stof(values[S_IRQ]) + 
          stof(values[S_SOFTIRQ]) + 
          stof(values[S_STEAL]) + 
          stof(values[S_GUEST]) + 
          stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string> values) {
  return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2) {
  float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
  float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
  float totalTime = activeTime + idleTime;
  float result = 100.0*(activeTime / totalTime);
  return to_string(result);
}

float ProcessParser::getSysRamPercent() {
  string line;
  float ram_percent;

  float buffers = 0;
  float total_mem = 0;
  float free_mem = 0;

  string memAvail = "MemAvailable:";
  string memFree = "MemFree:";
  string buffer = "Buffers:";

  ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
  while(getline(stream, line)) {
    if (line.compare(0, memAvail.size(), memAvail) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      total_mem = stof(values[1]);
    }
    if(line.compare(0, memFree.size(), memFree) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> result(beg, end);
      free_mem = stof(result[1]);
    }
    if(line.compare(0, buffer.size(), buffer) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> result(beg, end);
      buffers = stof(result[1]);
    }
  }
  ram_percent = float(100.0 * (1 - (free_mem/(total_mem - buffers))));
  return ram_percent;
}

string ProcessParser::getSysKernelVersion() {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> result(beg, end);
  return result[2];
}

string ProcessParser::getOSName() {
  string line;
  ifstream stream = Util::getStream("/etc/os-release");
  string name = "PRETTY_NAME";
  string result;
  
  while(getline(stream,line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result = values[0] + " " + values[1] + " " + values[2];
      break;
    }
  }
  result.erase(0,13);
  result.erase(result.end()-1);
  return result;
}

int ProcessParser::getTotalThreads() {
  string line;
  int result = 0;
  string name = "Threads";
  vector<string> pid_list = getPidList();
  
  for(string & pid : pid_list) {
    //cout<<(Path::basePath() + pid + Path::statusPath())<<endl;
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    while(getline(stream, line)) {
      if(line.compare(0, name.size(), name) == 0) {
        istringstream buf(line);
        istream_iterator<string> beg(buf), end;
        vector<string> values(beg, end);
        result += stoi(values[1]);
        break;
      }
    }
  }
  return result;
}

int ProcessParser::getTotalNumberOfProcesses() {
  string line;
  int result = 0;
  string name = "processes";
  
  ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result = stoi(values[1]);
      break;
    }
  }
  return result;
}

int ProcessParser::getNumberOfRunningProcesses() {
  string line;
  int result = 0;
  string name = "procs_running";
  
  ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result = stoi(values[1]);
      break;
    }
  }
  return result;
}

bool ProcessParser::isPidExisting(string pid) {
  bool exist = false;
  vector<string> pids = getPidList();
  for (auto & item : pids) {
    if(item == pid) exist = true;
  }
  return exist;
}
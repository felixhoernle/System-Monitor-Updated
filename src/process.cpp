#include "process.h"
#include "linux_parser.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { 
  
  return pid_; 
}

// Return this process's CPU utilization
float Process::CpuUtilization() const { 

  //active_time_ = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  //seconds_ = LinuxParser::UpTime() - Process::UpTime();
  //utilization_ = std::abs((active_time_ - prev_active_time_) / (seconds_ - prev_seconds_));
  //prev_active_time_ = active_time_;
  //prev_seconds_ = seconds_;
  
  float active_time = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  float seconds = LinuxParser::UpTime() - Process::UpTime();
  return active_time / seconds;
}

// Return the command that generated this process
string Process::Command() { 
  
  return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// Return the user (name) that generated this process
string Process::User() { 

  return LinuxParser::User(pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() const {
  
  return LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  
  return a.CpuUtilization() < this->CpuUtilization();
}

 void Process::setPid(int pid) {pid_ = pid;}
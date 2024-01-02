#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Construct a class object
Process::Process (int pid) {

  pid_ = pid;
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  float activeTime = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  float seconds = Process::UpTime();
  return activeTime / seconds;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() {
  long ramKB = stol(LinuxParser::Ram(pid_));
  return to_string(ramKB / 1024);
}

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() const {
  return LinuxParser::UpTime() -
         (LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK));
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < this->CpuUtilization();
}
#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Template function to extract a value from a filestream using a key
template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream filestream(LinuxParser::kProcDirectory + filename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          filestream.close();
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
};

// Template function to extract a value from a filestream without key
template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream filestream(LinuxParser::kProcDirectory + filename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  filestream.close();
  return value;
};

// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  std::string line, key, value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  auto memTotal =
      stof(findValueByKey<std::string>(filterMemTotalString, kMeminfoFilename));
  auto memFree =
      stof(findValueByKey<std::string>(filterMemFreeString, kMeminfoFilename));
  return (memTotal - memFree) / memTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  auto uptime = getValueOfFile<std::string>(kUptimeFilename);
  return stol(uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::string value, line;
  long activeJiffies = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i <= 17; i++) {
      linestream >> value;
      if (i == 14) activeJiffies += stol(value);
      if (i == 15) activeJiffies += stol(value);
      if (i == 16) activeJiffies += stol(value);
      if (i == 17) activeJiffies += stol(value);
    }
  }
  filestream.close();
  return activeJiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  return (stol(values[kUser_]) + stol(values[kNice_]) + stol(values[kSystem_]) +
          stol(values[kIRQ_]) + stol(values[kSoftIRQ_]) +
          stol(values[kSteal_]));
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  return (stol(values[kIdle_]) + stol(values[kIOwait_]));
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::string key, line, user, nice, system, idle, iowait, irq, softirq, steal,
      guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
           irq >> softirq >> steal >> guest >> guest_nice) {
      if (key == filterCpu) {
        filestream.close();
        return {user, nice,    system, idle,  iowait,
                irq,  softirq, steal,  guest, guest_nice};
      }
    }
  }
  return {};  // If nothing has been created, return empty vector
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return stoi(findValueByKey<std::string>(filterProcesses, kStatFilename));
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return stoi(
      findValueByKey<std::string>(filterRunningProcesses, kStatFilename));
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  return getValueOfFile<std::string>(to_string(pid) + kCmdlineFilename);
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  return findValueByKey<std::string>(filterProcMem,
                                     to_string(pid) + kStatusFilename);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  return findValueByKey<std::string>(filterUID,
                                     to_string(pid) + kStatusFilename);
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string line, user, passwd, value;
  std::string uid = Uid(pid);  // The user ID we are searching for
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> passwd >> value) {
        if (value == uid) {
          filestream.close();
          return user;
        }
      }
    }
  }
  return "?";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line, uptime;
  int counter;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      counter = 0;
      while (linestream >> uptime) {
        counter += 1;
        if (counter == 22) {
          filestream.close();
          return stol(uptime);
        }
      }
    }
  }
  return 0;
}

#include "processor.h"

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  activeJiffies_ = LinuxParser::ActiveJiffies();
  jiffies_ = LinuxParser::Jiffies();
  utilization_ = std::abs((float)(activeJiffies_ - prevActiveJiffies_) /
                          (float)(jiffies_ - prevJiffies_));
  prevActiveJiffies_ = activeJiffies_;
  prevJiffies_ = jiffies_;

  return utilization_;
}
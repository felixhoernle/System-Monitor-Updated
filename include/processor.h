#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

  // Declare any necessary private members
 private:
  float utilization_;
  long activeJiffies_;
  long jiffies_;
  long prevActiveJiffies_ = 0.0;
  long prevJiffies_ = 0.0;
};

#endif
#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float utilization_;
  long activejiffies_;
  long jiffies_;
  long prevactivejiffies_ = 0.0;
  long prevjiffies_ = 0.0;
};

#endif
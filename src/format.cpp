#include "format.h"

#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  seconds -= hours * 3600;

  long minutes = seconds / 60;
  seconds -= minutes * 60;

  // Create a C-string, print elapsed time in output format to it and then
  // convert it to a std::string
  char c[9]; // C-string has to have space for time plus terminating null character
  snprintf(c, 9, "%02ld:%02ld:%02ld", hours, minutes, seconds);
  string s(c);

  return s;
}
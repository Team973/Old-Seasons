#ifndef FRC971_WPILIB_GYRO_H_
#define FRC971_WPILIB_GYRO_H_

#include <stdint.h>

#include <atomic>

#include "gyro_interface.hpp"

namespace frc973 {

// Handles reading the gyro over SPI and sending out angles on a queue.
//
// This is designed to be passed into ::std::thread's constructor so it will run
// as a separate thread.
class GyroSender {
 public:
  GyroSender();

  static void* processor_executor(void *arg)
  {
      GyroSender *sender= static_cast<GyroSender*>(arg);
      (*sender)();
      return NULL;
  }

  // For ::std::thread to call.
  //
  // Initializes the gyro and then loops forever taking readings.
  void operator()();

  //static void Quit() { run_ = false; }

 private:

  // Readings per second.
  const unsigned int kReadingRate = 200;

  GyroInterface gyro_;

  const bool run_ = true;
};

}  // namespace wpilib

#endif  // FRC971_WPILIB_GYRO_H_

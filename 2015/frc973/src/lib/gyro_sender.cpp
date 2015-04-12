#include "gyro_sender.hpp"

#include <inttypes.h>
#include <unistd.h>


namespace frc973 {

GyroSender::GyroSender() {}

void GyroSender::operator()() {

  // Try to initialize repeatedly as long as we're supposed to be running.
  while (run_ && !gyro_.InitializeGyro()) {
    usleep(50000);
  }
  printf("gyro initialized successfully\n");

  //int message =  gyro_.ReadPartID();
  //printf("gyro ID %d\n", message);

  // In radians, ready to send out.
  double angle = 0;

  int startup_cycles_left = 2 * kReadingRate;

  double zeroing_data[6 * kReadingRate];
  size_t zeroing_index = 0;
  bool zeroed = false;
  bool have_zeroing_data = false;
  double zero_offset = 0;

  while (run_) {
    usleep(1000000.0 / kReadingRate);

    const uint32_t result = gyro_.GetReading();
    if (result == 0) {
      printf("normal gyro read failed\n");
      continue;
    }
    switch (gyro_.ExtractStatus(result)) {
      case 0:
        printf("gyro says data is bad; status=0x%X result=0x%X\n",
        		gyro_.ExtractStatus(result), result);
        continue;
      case 1:
        break;
      default:
        printf("gyro gave weird status 0x%X\n",
            gyro_.ExtractStatus(result));
        continue;
    }
    if (gyro_.ExtractErrors(result) != 0) {
      const uint8_t errors = gyro_.ExtractErrors(result);
      if (errors & (1 << 6)) {
        printf("gyro gave PLL error\n");
      }
      if (errors & (1 << 5)) {
        printf("gyro gave quadrature error\n");
      }
      if (errors & (1 << 4)) {
        printf("gyro gave non-volatile memory error\n");
      }
      if (errors & (1 << 3)) {
        printf("gyro gave volatile memory error\n");
      }
      if (errors & (1 << 2)) {
        printf("gyro gave power error\n");
      }
      if (errors & (1 << 1)) {
        printf("gyro gave continuous self-test error\n");
      }
      if (errors & 1) {
        printf("gyro gave unexpected self-test mode\n");
      }
      //printf("Fault1 0x%X\n", gyro_.DoRead(0x0A));
      //printf("Fault0 0x%X\n", gyro_.DoRead(0x0B));
      continue;
    }

    if (startup_cycles_left > 0) {
      --startup_cycles_left;
      continue;
    }

    const double new_angle =
        gyro_.ExtractAngle(result) / static_cast<double>(kReadingRate);
    //printf("sending %f from flat of %d\n", angle, gyro_.ExtractAngle(result));
    if (zeroed) {
      angle += new_angle;
      angle += zero_offset;
      printf("Angle: %f\n", angle);
      SmartDashboard::PutNumber("Gyro angle: ", angle);
    } else {
      // TODO(brian): Don't break without 6 seconds of standing still before
      // enabling. Ideas:
      //   Don't allow driving until we have at least some data?
      //   Some kind of indicator light?
      zeroing_data[zeroing_index] = new_angle;
      ++zeroing_index;
      if (zeroing_index >= 6 * kReadingRate) {
        zeroing_index = 0;
        have_zeroing_data = true;
      }

      if (have_zeroing_data) {
        zero_offset = 0;
        for (unsigned int i = 0; i < 6 * kReadingRate;
             ++i) {
          zero_offset -= zeroing_data[i];
        }
        printf("sum of zeros = %f", zero_offset);
        zero_offset /= static_cast<double>(6 * kReadingRate);
        printf("total zero offset %f\n", zero_offset);
        zeroed = true;
      }
    }
  }
}

}  // namespace wpilib

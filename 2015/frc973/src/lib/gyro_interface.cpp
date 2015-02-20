#include "gyro_interface.hpp"

#include <inttypes.h>
#include <unistd.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace frc973 {

GyroInterface::GyroInterface() : gyro_(new SPI(SPI::kOnboardCS0)) {
  // The gyro goes up to 8.08MHz.
  // The myRIO goes up to 4MHz, so the roboRIO probably does too.
  gyro_->SetClockRate(4e6);
  gyro_->SetChipSelectActiveLow();
  gyro_->SetClockActiveHigh();
  gyro_->SetSampleDataOnRising();
  gyro_->SetMSBFirst();
}

bool GyroInterface::InitializeGyro() {
  uint32_t result;
  if (!DoTransaction(0x20000003, &result)) {
    printf("failed to start a self-check\n");
    return false;
  }
  if (result != 1) {
    // We might have hit a parity error or something and are now retrying, so
    // this isn't a very big deal.
    printf("gyro unexpected initial response 0x%X\n", result);
  }

  // Wait for it to assert the fault conditions before reading them.
  if (usleep(50000))
	  printf("usleep failed somehow\n");

  if (!DoTransaction(0x20000000, &result)) {
    printf("failed to clear latched non-fault data\n");
    return false;
  }
  printf("gyro dummy response is 0x%X\n", result);

  if (!DoTransaction(0x20000000, &result)) {
    printf("failed to read self-test data\n");
    return false;
  }
  if (ExtractStatus(result) != 2) {
    printf("gyro first value 0x%X not self-test data\n", result);
    return false;
  }
  if (ExtractErrors(result) != 0x7F) {
    printf("gyro first value 0x%X does not have all errors\n",
        result);
    return false;
  }

  if (!DoTransaction(0x20000000, &result)) {
    printf("failed to clear latched self-test data\n");
    return false;
  }
  if (ExtractStatus(result) != 2) {
    printf("gyro second value 0x%X not self-test data\n",
        result);
    return false;
  }

  return true;
}

bool GyroInterface::DoTransaction(uint32_t to_write, uint32_t *result) {
  static const uint8_t kBytes = 4;
  static_assert(kBytes == sizeof(to_write),
                "need the same number of bytes as sizeof(the data)");

  if (__builtin_parity(to_write & ~1) == 0) to_write |= 1;

  uint8_t to_send[kBytes], to_receive[kBytes];
  const uint32_t to_write_flipped = __builtin_bswap32(to_write);
  memcpy(to_send, &to_write_flipped, kBytes);

  switch (gyro_->Transaction(to_send, to_receive, kBytes)) {
    case -1:
      printf("SPI::Transaction failed\n");
      return false;
    case kBytes:
      break;
    default:
      printf("SPI::Transaction returned something weird\n");
  }

  memcpy(result, to_receive, kBytes);
  if (__builtin_parity(*result & 0xFFFF) != 1) {
    printf("high byte parity failure\n");
    return false;
  }
  if (__builtin_parity(*result) != 1) {
    printf("whole value parity failure\n");
    return false;
  }

  *result = __builtin_bswap32(*result);
  return true;
}

uint16_t GyroInterface::DoRead(uint8_t address) {
  const uint32_t command = (0x8 << 28) | (address << 17);
  uint32_t response;
  while (true) {
    if (!DoTransaction(command, &response)) {
      printf("reading 0x%X failed\n", address);
      continue;
    }
    if ((response & 0xEFE00000) != 0x4E000000) {
      printf("gyro read from 0x%X gave unexpected response 0x%X\n",
          address, response);
      continue;
    }
    return (response >> 5) & 0xFFFF;
  }
}

double GyroInterface::ExtractAngle(uint32_t value) {
  const int16_t reading = -(int16_t)(value >> 10 & 0xFFFF);
  return static_cast<double>(reading) / 80.0;
}

uint32_t GyroInterface::ReadPartID() {
  return (DoRead(0x0E) << 16) | DoRead(0x10);
}

uint32_t GyroInterface::GetReading() {
  uint32_t result;
  if (!DoTransaction(0x20000000, &result)) {
    return 0;
  }
  return result;
}

}

#include "flagAccumulator.hpp"

namespace frc973 {

FlagAccumulator::FlagAccumulator() {
    threshold = 0;
    flagCount = 0;
}

void FlagAccumulator::reset() {
    flagCount = 0;
}

void FlagAccumulator::setThreshold(int threshold_) {
    threshold = threshold_;
}

bool FlagAccumulator::update(bool current) {
    if (current) {
        flagCount += 1;
    }

    if (flagCount >= threshold) {
        return true;
    }

    return false;
}

}

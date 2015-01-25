#ifndef FLAG_ACCUMULATOR_HPP
#define FLAG_ACCUMULATOR_HPP

namespace frc973 {

class FlagAccumulator {
public:
    FlagAccumulator();
    void setThreshold(int threshold_);
    bool update(bool current);
private:
    int threshold;
    int flagCount;
};

}

#endif

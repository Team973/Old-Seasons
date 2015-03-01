#ifndef FLAG_ACCUMULATOR_HPP
#define FLAG_ACCUMULATOR_HPP

namespace frc973 {

class FlagAccumulator {
public:
    FlagAccumulator();
    void reset();
    void setThreshold(int threshold_);
    int getFlagCount();
    bool update(bool current);
private:
    int threshold;
    int flagCount;
};

}

#endif

#ifndef CONSTANTS_BASE_HPP
#define CONSTANTS_BASE_HPP

#include <string>
#include "../utility.hpp"
#include <vector>

class ConstantsBase
{
public:
    ConstantsBase();
    void readConstantsFile();
    virtual ~ConstantsBase(){}

    class Constant
    {
        public:
            Constant(std::string name_, double value_)
            {
                name = asLower(name_);
                value = value_;
                constants.push_back(this);
            }

            std::string getName()
            {
                return name;
            }

            double getDouble()
            {
                return value;
            }

            int getInt()
            {
                return (int)value;
            }

            void setValue(double value_)
            {
                value = value_;
            }

            std::string toString()
            {
                return name + " = " + asString(value);
            }

            virtual ~Constant(){}
        private:
            std::string name;
            double value;
    };

    static std::vector<Constant*> constants;
private:
};
#endif

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include "lib/utility.hpp"
#include <vector>

namespace frc973 {
    
class Constants {
public:
    Constants();
    void Extend();
    void readConstantsFile();
    void addConstant(std::string name, double value);

    class Constant
    {
        public:
            Constant(std::string name_, double value_)
            {
                name = asLower(name_);
                value = value_;
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

    Constant* getConstant(std::string name);

    static Constants *instance;

private:
    std::vector<Constant*> constants;
};

}
#endif

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include "lib/utility.hpp"
#include "lib/socketClient.hpp"
#include <vector>

namespace frc973 {
    
class Constants {
public:
    static void Extend();
    static void readConstantsFile();
    static void addConstant(std::string name, double value);

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

            float getFloat()
            {
                return (float)value;
            }

            int getInt()
            {
                return (int)value;
            }

            void setValue(double value_)
            {
                value = value_;
				SocketClient::UpdateDash(name, value);
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

    static Constant* getConstant(std::string name);

private:
    static std::vector<Constant*> constants;
};

}
#endif

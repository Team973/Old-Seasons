#ifndef AUTO_SEQUENCER_HPP
#define AUTO_SEQUENCER_HPP

#include <vector>
#include "autoCommand.hpp"
#include <string>

namespace frc973 {

class AutoSequencer : public AutoCommand {
public:
    AutoSequencer(std::string name_);
    std::string getName();
    void addSequential(AutoCommand* command);
    void addConcurrent(AutoCommand* command);
    void init();
    bool run();
private:

    struct COMMAND {
        std::string type;
        AutoCommand* cmd;
    };

    void addCommand(AutoSequencer::COMMAND* command, int order=-1);
    AutoSequencer::COMMAND* newCommand(AutoCommand* command, std::string type);
    std::vector< std::vector<AutoSequencer::COMMAND*> > sequence;

    std::string name;
};

}

#endif

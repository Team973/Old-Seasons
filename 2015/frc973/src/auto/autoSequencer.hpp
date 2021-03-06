#ifndef AUTO_SEQUENCER_HPP
#define AUTO_SEQUENCER_HPP

#include <vector>
#include "commands/autoCommand.hpp"
#include <string>

namespace frc973 {

class AutoSequencer : public AutoCommand {
public:
    AutoSequencer();
    void addSequential(AutoCommand* command);
    void addConcurrent(AutoCommand* command);
    void init();
    bool taskPeriodic();
private:

    struct COMMAND {
        std::string type;
        AutoCommand* cmd;
    };

    void addCommand(AutoSequencer::COMMAND* command);
    AutoSequencer::COMMAND* newCommand(AutoCommand* command, std::string type);
    std::vector< std::vector<AutoSequencer::COMMAND*> > sequence;

    unsigned int currOuterCommand;
};

}

#endif

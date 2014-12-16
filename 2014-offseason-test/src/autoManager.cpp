#include "autoManager.hpp"
#include "auto/autoCommand.hpp"
#include "auto/waitCommand.hpp"

namespace frc973 {

AutoManager::AutoManager()
{
    modes["Test"] = new AutoSequencer();

    modes["Test"]->addSequential(new WaitCommand(1));
}

}

#include "autoManager.hpp"
#include "auto/autoCommand.hpp"

AutoManager::AutoManager()
{
    modes["Test"] = new AutoSequencer();

    //modes["Test"]->addSequential();
}

#include "autoManager.hpp"
#include "auto/autoCommand.hpp"

namespace frc973 {

AutoManager::AutoManager()
{
    modes["Test"] = new AutoSequencer();

    //modes["Test"]->addSequential();
}

}

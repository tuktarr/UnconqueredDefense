#include "TowerDefense.h"
#include "Modules/ModuleManager.h"

void StartTDMigrationSmoke();
void StopTDMigrationSmoke();
class FTowerDefenseModule : public FDefaultGameModuleImpl
{
public:
    virtual void StartupModule() override { StartTDMigrationSmoke(); }
    virtual void ShutdownModule() override { StopTDMigrationSmoke(); }
};
IMPLEMENT_PRIMARY_GAME_MODULE(FTowerDefenseModule, TowerDefense, "TowerDefense");

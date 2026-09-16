using UnrealBuildTool;
using System.Collections.Generic;

public class TowerDefenseTarget : TargetRules
{
    public TowerDefenseTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        ExtraModuleNames.Add("TowerDefense");
    }
}

using UnrealBuildTool;
using System.Collections.Generic;

public class TowerDefenseEditorTarget : TargetRules
{
    public TowerDefenseEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        ExtraModuleNames.Add("TowerDefense");
    }
}

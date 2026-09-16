using UnrealBuildTool;

public class TowerDefense : ModuleRules
{
    public TowerDefense(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // Feature folders contain both headers and implementations.
        PublicIncludePaths.Add(ModuleDirectory);
        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AIModule"
        });
    }
}

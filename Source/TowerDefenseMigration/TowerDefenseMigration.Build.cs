using UnrealBuildTool;

public class TowerDefenseMigration : ModuleRules
{
    public TowerDefenseMigration(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePaths.Add(ModuleDirectory);
        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core", "CoreUObject", "Engine", "UnrealEd", "BlueprintGraph",
            "KismetCompiler", "AssetRegistry", "Json", "TowerDefense"
        });
    }
}

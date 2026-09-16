#include "Commandlets/Migrate/TDMigrateCommandlet.h"
#include "BlueprintBridge/TDGameplayLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "K2Node_CallFunction.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_Self.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "KismetCompiler.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/SavePackage.h"

UTDMigrateCommandlet::UTDMigrateCommandlet()
{
    IsClient=false; IsServer=false; IsEditor=true; LogToConsole=true;
}

int32 UTDMigrateCommandlet::Main(const FString& Params)
{
    FString ManifestPath;
    if (!FParse::Value(*Params, TEXT("Manifest="), ManifestPath)) return 1;
    FString Text;
    TSharedPtr<FJsonObject> Manifest;
    if (!FFileHelper::LoadFileToString(Text, *ManifestPath) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Manifest)) return 1;
    TSet<UBlueprint*> Changed;
    const UEdGraphSchema_K2* Schema=GetDefault<UEdGraphSchema_K2>();
    for (const auto& Entry : Manifest->GetArrayField(TEXT("routes")))
    {
        auto Route=Entry->AsObject();
        UBlueprint* BP=LoadObject<UBlueprint>(nullptr, *Route->GetStringField(TEXT("asset")));
        if (!BP) return 2;
        TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs);
        UEdGraph* Graph=nullptr;
        for (UEdGraph* G : Graphs) if (G->GetName()==Route->GetStringField(TEXT("graph"))) Graph=G;
        if (!Graph) return 3;
        UEdGraphNode* Start=nullptr;
        for (UEdGraphNode* N : Graph->Nodes) if (N && N->GetName()==Route->GetStringField(TEXT("entry"))) Start=N;
        if (!Start) return 4;
        const FString FunctionName=Route->GetStringField(TEXT("native"));
        const FString Marker=TEXT("TD_NATIVE:")+FunctionName+TEXT(":")+Start->GetName();
        bool Existing=false;
        for (UEdGraphNode* N : Graph->Nodes) if (N && N->NodeComment==Marker) Existing=true;
        if (Existing) continue;
        UFunction* Function=UTDGameplayLibrary::StaticClass()->FindFunctionByName(FName(*FunctionName));
        if (!Function) return 5;
        FString ExecName=TEXT("then"); Route->TryGetStringField(TEXT("exec"), ExecName);
        UEdGraphPin* Exec=Start->FindPin(FName(*ExecName));
        if (!Exec) return 6;
        UEdGraphPin* ResultExec=nullptr;
        UK2Node_FunctionResult* Result=nullptr;
        // Function signatures stay unchanged: only their implementation is replaced.
        if (Cast<UK2Node_FunctionEntry>(Start))
            for (UEdGraphNode* N : Graph->Nodes) if (auto* R=Cast<UK2Node_FunctionResult>(N)) { Result=R; break; }
        auto* Call=NewObject<UK2Node_CallFunction>(Graph);
        Graph->AddNode(Call, false, false);
        Call->CreateNewGuid(); Call->SetFromFunction(Function); Call->AllocateDefaultPins();
        Call->NodePosX=Start->NodePosX+400; Call->NodePosY=Start->NodePosY;
        Call->NodeComment=Marker;
        auto* Self=NewObject<UK2Node_Self>(Graph);
        Graph->AddNode(Self,false,false); Self->CreateNewGuid(); Self->AllocateDefaultPins();
        Self->NodePosX=Call->NodePosX-150; Self->NodePosY=Call->NodePosY+150;
        if (!Schema->TryCreateConnection(Self->FindPinChecked(UEdGraphSchema_K2::PN_Self), Call->FindPinChecked(TEXT("Context")))) return 7;
        for (UEdGraphPin* Pin : Call->Pins)
        {
            if (Pin->PinType.PinCategory==UEdGraphSchema_K2::PC_Exec || Pin->PinName==TEXT("Context") || Pin->PinName==UEdGraphSchema_K2::PN_Self) continue;
            if (Pin->Direction==EGPD_Input)
            {
                UEdGraphPin* Source=Start->FindPin(Pin->PinName);
                if (!Source || !Schema->TryCreateConnection(Source, Pin))
                {
                    UE_LOG(LogTemp,Error,TEXT("Missing/incompatible input %s.%s -> %s"),*Start->GetName(),*Pin->PinName.ToString(),*FunctionName);
                    return 8;
                }
            }
            else if (Result)
            {
                if (UEdGraphPin* Destination=Result->FindPin(Pin->PinName))
                {
                    Destination->BreakAllPinLinks();
                    if (!Schema->TryCreateConnection(Pin, Destination)) return 9;
                }
            }
        }
        Exec->BreakAllPinLinks();
        if (!Schema->TryCreateConnection(Exec, Call->FindPinChecked(UEdGraphSchema_K2::PN_Execute))) return 10;
        if (Result)
        {
            ResultExec=Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute);
            ResultExec->BreakAllPinLinks();
            if (!Schema->TryCreateConnection(Call->FindPinChecked(UEdGraphSchema_K2::PN_Then), ResultExec)) return 11;
        }
        FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
        Changed.Add(BP);
        UE_LOG(LogTemp, Display, TEXT("Routed %s:%s:%s -> %s"), *BP->GetName(), *Graph->GetName(), *Start->GetName(), *FunctionName);
    }
    // Compile every affected asset before saving any changes.
    for (UBlueprint* BP : Changed)
    {
        FCompilerResultsLog Results;
        FKismetEditorUtilities::CompileBlueprint(BP, EBlueprintCompileOptions::None, &Results);
        if (Results.NumErrors) { UE_LOG(LogTemp, Error, TEXT("Compile failed: %s"), *BP->GetName()); return 12; }
    }
    // Include dependent widget, animation and child Blueprints, not just edited graphs.
    auto& Registry=FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    Registry.SearchAllAssets(true);
    TArray<FAssetData> Assets;
    Registry.GetAssetsByPath(TEXT("/Game/Blueprint"), Assets, true);
    int32 Compiled=0;
    for (const FAssetData& Asset : Assets)
        if (auto* BP=Cast<UBlueprint>(Asset.GetAsset()))
        {
            FCompilerResultsLog Results;
            FKismetEditorUtilities::CompileBlueprint(BP,EBlueprintCompileOptions::None,&Results);
            if (Results.NumErrors) { UE_LOG(LogTemp,Error,TEXT("Dependent compile failed: %s"),*BP->GetPathName()); return 15; }
            ++Compiled;
        }
    UE_LOG(LogTemp,Display,TEXT("Compiled %d project Blueprints including dependents."),Compiled);
    if (!FParse::Param(*Params, TEXT("Apply")))
    {
        UE_LOG(LogTemp, Display, TEXT("Dry run: %d Blueprints compiled; no asset saved."), Changed.Num());
        return 0;
    }
    for (UBlueprint* BP : Changed)
    {
        UPackage* Package=BP->GetOutermost();
        const FString Filename=FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
        const FString Backup=FPaths::ProjectSavedDir()/TEXT("BlueprintMigration/Originals")/FPaths::GetCleanFilename(Filename);
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(Backup), true);
        if (!IFileManager::Get().FileExists(*Backup) && IFileManager::Get().Copy(*Backup,*Filename)!=COPY_OK) return 13;
        FSavePackageArgs Args; Args.TopLevelFlags=RF_Public|RF_Standalone; Args.SaveFlags=SAVE_NoError;
        if (!UPackage::SavePackage(Package,BP,*Filename,Args)) return 14;
    }
    UE_LOG(LogTemp, Display, TEXT("Saved %d migrated Blueprints."), Changed.Num());
    return 0;
}

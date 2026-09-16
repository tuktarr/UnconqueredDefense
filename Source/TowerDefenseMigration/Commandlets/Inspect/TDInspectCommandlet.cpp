#include "Commandlets/Inspect/TDInspectCommandlet.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/UnrealType.h"

UTDInspectCommandlet::UTDInspectCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}

int32 UTDInspectCommandlet::Main(const FString& Params)
{
    FString Output = FPaths::ProjectSavedDir() / TEXT("BlueprintMigration/Inventory.json");
    FParse::Value(*Params, TEXT("Output="), Output);
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    Registry.SearchAllAssets(true);
    TArray<FAssetData> Assets;
    Registry.GetAssetsByPath(TEXT("/Game/Blueprint"), Assets, true);
    TArray<TSharedPtr<FJsonValue>> Results;
    for (const FAssetData& Asset : Assets)
    {
        UObject* Object = Asset.GetAsset();
        UBlueprint* BP = Cast<UBlueprint>(Object);
        UEnum* Enum = Cast<UEnum>(Object);
        UDataTable* Table = Cast<UDataTable>(Object);
        if (!BP && !Enum && !Table) continue;
        auto Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("path"), Object->GetPathName());
        if (Enum)
        {
            TArray<TSharedPtr<FJsonValue>> Values;
            for (int32 Index = 0; Index < Enum->NumEnums(); ++Index)
                Values.Add(MakeShared<FJsonValueString>(Enum->GetNameStringByIndex(Index) + TEXT("=") + Enum->GetDisplayNameTextByIndex(Index).ToString()));
            Item->SetArrayField(TEXT("enum"), Values);
        }
        if (Table) Item->SetStringField(TEXT("table"), Table->GetTableAsJSON(EDataTableExportFlags::UseJsonObjectsForStructs));
        if (BP)
        {
            Item->SetStringField(TEXT("parent"), GetPathNameSafe(BP->ParentClass));
            auto Defaults = MakeShared<FJsonObject>();
            UObject* CDO = BP->GeneratedClass ? BP->GeneratedClass->GetDefaultObject() : nullptr;
            if (CDO)
            {
                for (TFieldIterator<FProperty> It(CDO->GetClass()); It; ++It)
                {
                    FString Value;
                    It->ExportText_InContainer(0, Value, CDO, CDO, CDO, PPF_None);
                    Defaults->SetStringField(It->GetName(), Value);
                }
            }
            Item->SetObjectField(TEXT("defaults"), Defaults);
            TArray<UEdGraph*> Graphs;
            BP->GetAllGraphs(Graphs);
            TArray<TSharedPtr<FJsonValue>> GraphResults;
            for (UEdGraph* Graph : Graphs)
            {
                auto G = MakeShared<FJsonObject>();
                G->SetStringField(TEXT("name"), Graph->GetName());
                TArray<TSharedPtr<FJsonValue>> Nodes;
                for (UEdGraphNode* Node : Graph->Nodes)
                {
                    if (!Node) continue;
                    auto N = MakeShared<FJsonObject>();
                    N->SetStringField(TEXT("id"), Node->GetName());
                    N->SetStringField(TEXT("class"), Node->GetClass()->GetName());
                    N->SetStringField(TEXT("title"), Node->GetNodeTitle(ENodeTitleType::ListView).ToString());
                    N->SetStringField(TEXT("comment"), Node->NodeComment);
                    TArray<TSharedPtr<FJsonValue>> Pins;
                    for (UEdGraphPin* Pin : Node->Pins)
                    {
                        auto P = MakeShared<FJsonObject>();
                        P->SetStringField(TEXT("name"), Pin->PinName.ToString());
                        P->SetStringField(TEXT("direction"), Pin->Direction == EGPD_Input ? TEXT("in") : TEXT("out"));
                        P->SetStringField(TEXT("type"), Pin->PinType.PinCategory.ToString());
                        P->SetStringField(TEXT("subtype"), GetPathNameSafe(Pin->PinType.PinSubCategoryObject.Get()));
                        P->SetStringField(TEXT("default"), Pin->DefaultValue);
                        P->SetStringField(TEXT("defaultText"), Pin->DefaultTextValue.ToString());
                        P->SetStringField(TEXT("object"), GetPathNameSafe(Pin->DefaultObject));
                        TArray<TSharedPtr<FJsonValue>> Links;
                        for (UEdGraphPin* Linked : Pin->LinkedTo)
                            Links.Add(MakeShared<FJsonValueString>(Linked->GetOwningNode()->GetName() + TEXT(".") + Linked->PinName.ToString()));
                        P->SetArrayField(TEXT("links"), Links);
                        Pins.Add(MakeShared<FJsonValueObject>(P));
                    }
                    N->SetArrayField(TEXT("pins"), Pins);
                    Nodes.Add(MakeShared<FJsonValueObject>(N));
                }
                G->SetArrayField(TEXT("nodes"), Nodes);
                GraphResults.Add(MakeShared<FJsonValueObject>(G));
            }
            Item->SetArrayField(TEXT("graphs"), GraphResults);
        }
        Results.Add(MakeShared<FJsonValueObject>(Item));
    }
    auto Root = MakeShared<FJsonObject>();
    Root->SetArrayField(TEXT("assets"), Results);
    FString Json;
    FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Json));
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Output), true);
    if (!FFileHelper::SaveStringToFile(Json, *Output)) return 1;
    UE_LOG(LogTemp, Display, TEXT("Exported %d assets to %s"), Results.Num(), *Output);
    return 0;
}

#include "EditorEngine.h"

#include "World/World.h"
#include "Level.h"
#include "Actors/Cube.h"
#include "Actors/DirectionalLightActor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerController.h"
#include "Classes/Engine/AssetManager.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameInstance.h"

#include "Engine/Source/Runtime/Game/Data/MapManager.h"
#include "Engine/Source/Runtime/Game/Sound/SoundManager.h"

namespace PrivateEditorSelection
{
    static AActor* GActorSelected = nullptr;
    static AActor* GActorHovered = nullptr;

    static USceneComponent* GComponentSelected = nullptr;
    static USceneComponent* GComponentHovered = nullptr;
}

void UEditorEngine::Init()
{
    Super::Init();

    // Initialize the engine
    GEngine = this;

    FWorldContext& EditorWorldContext = CreateNewWorldContext(EWorldType::Editor);

    EditorWorld = UWorld::CreateWorld(this, EWorldType::Editor, FString("EditorWorld"));

    EditorWorldContext.SetCurrentWorld(EditorWorld);
    ActiveWorld = EditorWorld;

    EditorPlayer = FObjectFactory::ConstructObject<AEditorPlayer>(this);
    CurrentPlayer = EditorPlayer;
    if (AssetManager == nullptr)
    {
        AssetManager = FObjectFactory::ConstructObject<UAssetManager>(this);
        assert(AssetManager);
        AssetManager->InitAssetManager();
    }
#pragma region GameMode
    GameMode = FObjectFactory::ConstructObject<AGameMode>(this);
#pragma endregion

    if (MapManager == nullptr)
    {
        MapManager = FObjectFactory::ConstructObject<UMapManager>(this);
        assert(MapManager);
        MapManager->InitMapManager(FString("pacman_map.txt"));
    }

    if (SoundManager == nullptr)
    {
        SoundManager = FObjectFactory::ConstructObject<USoundManager>(this);
        assert(SoundManager);
        //SoundManager->InitSoundManager();
    }

#ifdef _DEBUG
    AActor* Actor = EditorWorld->SpawnActor<ACube>();
    
    ADirectionalLight* DirLight = EditorWorld->SpawnActor<ADirectionalLight>();
    DirLight->SetActorRotation(FRotator(20, -61, 11));
    DirLight->SetActorLocation(FVector(0, 0, 20));
    DirLight->SetIntensity(2.f);

    APawn* TestPawn = EditorWorld->SpawnActor<ADefaultPawn>();
#endif
}

void UEditorEngine::Tick(float DeltaTime)
{
    for (FWorldContext* WorldContext : WorldList)
    {
        if (WorldContext->WorldType == EWorldType::Editor)
        {
            if (UWorld* World = WorldContext->World())
            {
                // TODO: World에서 EditorPlayer 제거 후 Tick 호출 제거 필요.
                World->Tick(DeltaTime);
                EditorPlayer->Tick(DeltaTime);
                ULevel* Level = World->GetActiveLevel();
                TArray CachedActors = Level->Actors;
                if (Level)
                {
                    for (AActor* Actor : CachedActors)
                    {
                        if (Actor && Actor->IsActorTickInEditor())
                        {
                            Actor->Tick(DeltaTime);
                        }
                    }
                }
            }
        }
        else if (WorldContext->WorldType == EWorldType::PIE)
        {   
            if (UWorld* World = WorldContext->World())
            {
                World->Tick(DeltaTime);
                CurrentPlayer->Input();
                ULevel* Level = World->GetActiveLevel();
                TArray CachedActors = Level->Actors;
                if (Level)
                {
                    for (AActor* Actor : CachedActors)
                    {
                        if (Actor)
                        {
                            Actor->Tick(DeltaTime);
                        }
                    }
                    GEngineLoop.LuaCompiler.Tick(DeltaTime);
                }
            }
        }
    }
    Input();
}

void UEditorEngine::Input()
{
        if (GetAsyncKeyState(VK_F8) & 0x8000)
        {
            if (bF8Clicked == false)
            {
                bF8Clicked = true;
                if (ActiveWorld->WorldType == EWorldType::PIE)
                {
                    if (CurrentPlayer == EditorPlayer)
                        CurrentPlayer = PlayerController;
                    else
                    {
                        CurrentPlayer = EditorPlayer;
                    }
                }
            }
        }
        else
        {
            bF8Clicked = false; 
        }
}

void UEditorEngine::StartPIE()
{
    if (PIEWorld)
    {
        UE_LOG(ELogLevel::Warning, TEXT("PIEWorld already exists!"));
        return;
    }

    FWorldContext& PIEWorldContext = CreateNewWorldContext(EWorldType::PIE);

    PIEWorld = Cast<UWorld>(EditorWorld->Duplicate(this));
    PIEWorld->WorldType = EWorldType::PIE;

    PIEWorldContext.SetCurrentWorld(PIEWorld);
    ActiveWorld = PIEWorld;
    
    PIEWorld->BeginPlay();
    // 여기서 Actor들의 BeginPlay를 해줄지 안에서 해줄 지 고민.
    WorldList.Add(GetWorldContextFromWorld(PIEWorld));

    TArray<AActor*> Actors = PIEWorld->GetActiveLevel()->Actors;
    for (auto iter = Actors.begin(); iter != Actors.end(); iter++)
    {
        if (Cast<ADefaultPawn>(*iter))
        {
            UE_LOG(ELogLevel::Error, TEXT("Actor Test"));
            APawn* Pawn = Cast<APawn>(*iter);
            APlayerController* Controller = PIEWorld->SpawnActor<APlayerController>();
            Pawn->SetPossessedController(Controller);
            Controller->AttachtoPawn(Pawn);
            PlayerController = Controller;
            CurrentPlayer = PlayerController;
            break;
        }
    }
}

void UEditorEngine::EndPIE()
{
    if (PIEWorld)
    {
        //WorldList.Remove(*GetWorldContextFromWorld(PIEWorld.get()));
        WorldList.Remove(GetWorldContextFromWorld(PIEWorld));
        PIEWorld->Release();
        GUObjectArray.MarkRemoveObject(PIEWorld);
        PIEWorld = nullptr;

        // TODO: PIE에서 EditorWorld로 돌아올 때, 기존 선택된 Picking이 유지되어야 함. 현재는 에러를 막기위해 임시조치.
        SelectActor(nullptr);
        SelectComponent(nullptr);
    }
    // 다시 EditorWorld로 돌아옴.
    ActiveWorld = EditorWorld;
    CurrentPlayer = EditorPlayer;
}

void UEditorEngine::InitGame()
{
    GameInstance = FObjectFactory::ConstructObject<UGameInstance>(this);
    GameInstance->Init();
}

FWorldContext& UEditorEngine::GetEditorWorldContext(/*bool bEnsureIsGWorld*/)
{
    for (FWorldContext* WorldContext : WorldList)
    {
        if (WorldContext->WorldType == EWorldType::Editor)
        {
            return *WorldContext;
        }
    }
    return CreateNewWorldContext(EWorldType::Editor);
}

FWorldContext* UEditorEngine::GetPIEWorldContext(/*int32 WorldPIEInstance*/)
{
    for (FWorldContext* WorldContext : WorldList)
    {
        if (WorldContext->WorldType == EWorldType::PIE)
        {
            return WorldContext;
        }
    }
    return nullptr;
}

void UEditorEngine::SelectActor(AActor* InActor) const
{
    if (InActor && CanSelectActor(InActor))
    {
        PrivateEditorSelection::GActorSelected = InActor;
    }
    else if (InActor == nullptr)
    {
        PrivateEditorSelection::GActorSelected = nullptr;
    }
}

bool UEditorEngine::CanSelectActor(const AActor* InActor) const
{
    return InActor != nullptr && InActor->GetWorld() == ActiveWorld && !InActor->IsActorBeingDestroyed();
}

AActor* UEditorEngine::GetSelectedActor() const
{
    return PrivateEditorSelection::GActorSelected;
}

void UEditorEngine::HoverActor(AActor* InActor) const
{
    if (InActor)
    {
        PrivateEditorSelection::GActorHovered = InActor;
    }
    else if (InActor == nullptr)
    {
        PrivateEditorSelection::GActorHovered = nullptr;
    }
}

void UEditorEngine::NewWorld()
{
    SelectActor(nullptr);
    SelectComponent(nullptr);

    if (ActiveWorld->GetActiveLevel())
    {
        ActiveWorld->GetActiveLevel()->Release();
    }
}

void UEditorEngine::SelectComponent(USceneComponent* InComponent) const
{
    if (InComponent && CanSelectComponent(InComponent))
    {
        PrivateEditorSelection::GComponentSelected = InComponent;
    }
    else if (InComponent == nullptr)
    {
        PrivateEditorSelection::GComponentSelected = nullptr;
    }
}

bool UEditorEngine::CanSelectComponent(const USceneComponent* InComponent) const
{
    return InComponent != nullptr && InComponent->GetOwner() && InComponent->GetOwner()->GetWorld() == ActiveWorld && !InComponent->GetOwner()->IsActorBeingDestroyed();
}

USceneComponent* UEditorEngine::GetSelectedComponent() const
{
    return PrivateEditorSelection::GComponentSelected;
}

void UEditorEngine::HoverComponent(USceneComponent* InComponent) const
{
    if (InComponent)
    {
        PrivateEditorSelection::GComponentHovered = InComponent;
    }
    else if (InComponent == nullptr)
    {
        PrivateEditorSelection::GComponentHovered = nullptr;
    }
}

AEditorPlayer* UEditorEngine::GetEditorPlayer() const
{
    return EditorPlayer;
}

#include "GameMode.h"

#include "Engine/EditorEngine.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "World/World.h"


AGameMode::AGameMode()
{
    TransformComponent = AddComponent<USceneComponent>("TransformComponent_0");
    RootComponent = TransformComponent;
}

void AGameMode::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

void AGameMode::StartGame()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    TArray<AActor*> Actors = EditorEngine->PIEWorld->GetActiveLevel()->Actors;
    for (auto iter = Actors.begin(); iter != Actors.end(); iter++)
    {
        if (Cast<ADefaultPawn>(*iter))
        {
            UE_LOG(ELogLevel::Error, TEXT("Actor Test"));
            APawn* Pawn = Cast<APawn>(*iter);
            APlayerController* Controller = EditorEngine->PIEWorld->SpawnActor<APlayerController>();
            Pawn->SetPossessedController(Controller);
            Controller->AttachtoPawn(Pawn);
            EditorEngine->GetGameInstance()->GetLocalPlayer()->SwitchController(Controller);
            // CurrentPlayer = PlayerController;
            break;
        }
    }
}

void AGameMode::GameOver()
{
    //TODO : GameOver UI 띄우기 
}

void AGameMode::Win()
{
    //TODO : Game Clear UI 띄우기 
}

void AGameMode::RestartGame()
{
    Score = 0;
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine != nullptr)
    {
        EditorEngine->NewWorld();
        EditorEngine->LoadWorld(EditorEngine->ActiveWorld->GetWorldName());
        EditorEngine->ActiveWorld->BeginPlay();
    }
}

void AGameMode::AddScore(float InScore)
{
    Score += InScore;
}

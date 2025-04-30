#include "GameMode.h"

#include "Camera/CameraFadeInModifier.h"
#include "Camera/CameraLetterBoxIn.h"
#include "Camera/CameraLetterBoxOut.h"
#include "Camera/PlayerCameraManager.h"
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
            Controller->Initialize();
            EditorEngine->GetGameInstance()->GetLocalPlayer()->SwitchController(Controller);
            // CurrentPlayer = PlayerController;
            UCameraLetterBoxIn* LetterModi = FObjectFactory::ConstructObject<UCameraLetterBoxIn>(this);
            LetterModi->Initialize(2.0f);
            Cast<APlayerController>(Controller)->GetPlayerCameraManager()->AddCameraModifier(LetterModi);
            break;
        }
    }

}

void AGameMode::GameOver()
{
    //TODO : GameOver UI 띄우기
    HP--;
    if (HP<= 0)
    {
        bGameOver = true;
        UCameraLetterBoxOut* LetterOut = FObjectFactory::ConstructObject<UCameraLetterBoxOut>(this);
        LetterOut->Initialize(2.0f);
        Cast<UEditorEngine>(GEngine)->GetGameInstance()->GetLocalPlayer()->GetPlayerController()->GetPlayerCameraManager()->AddCameraModifier(LetterOut);
        UE_LOG(ELogLevel::Display, "GameOver");
    }
    else
    {
        UE_LOG(ELogLevel::Display, "Hp : %d", HP);
        UCameraFadeInModifier* FadeInModifier = FObjectFactory::ConstructObject<UCameraFadeInModifier>(this);
        FadeInModifier->Initialize(FLinearColor::Red, 2.0f);
        Cast<UEditorEngine>(GEngine)->GetGameInstance()->GetLocalPlayer()->GetPlayerController()->GetPlayerCameraManager()->AddCameraModifier(FadeInModifier);
    }
}

void AGameMode::Win()
{
    bWin = true;
    //TODO : Game Clear UI 띄우기
    UE_LOG(ELogLevel::Display, "Win");
}

void AGameMode::RestartGame()
{
    Score = 0;
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine != nullptr)
    {
        EditorEngine->EndPIE();
        EditorEngine->StartPIE();
    }
}

void AGameMode::AddScore(float InScore)
{
    Score += InScore;
    if (Score >= WinScore)
        Win();
    // UE_LOG(ELogLevel::Display, "Add %f", Score);
}

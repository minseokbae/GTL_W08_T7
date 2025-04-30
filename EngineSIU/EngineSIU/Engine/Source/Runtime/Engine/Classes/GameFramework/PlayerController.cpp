#include "PlayerController.h"

#include "Pawn.h"
#include "WindowsCursor.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraFadeInModifier.h"
#include "Engine/EditorEngine.h"
#include "Engine/Engine.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"
#include "World/World.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraModifier.h"
#include "Camera/CameraShakeBase.h"

#include "SlateCore/Input/Events.h"

APlayerController::APlayerController()
    : Player(nullptr)
    , PlayerCameraManager(nullptr)
    , CameraShakeModifier(nullptr)
    , MyMessageHandler(nullptr)
    , bIsRunning(false)
{
    MouseCenterPos = { 0, 0 };
}

APlayerController::~APlayerController()
{
    MyMessageHandler->OnKeyDownDelegate.RemoveAllForObject(this);
    MyMessageHandler->OnKeyUpDelegate.RemoveAllForObject(this);

}

void APlayerController::Tick(float DeltaTime)
{
    AController::Tick(DeltaTime);
}

void APlayerController::Input()
{
    AController::Input();
    FWindowsCursor::SetShowMouseCursor(false);
    Pawn->GetRootComponent()->ComponentVelocity = FVector(0.f, 0.f, 0.f);
    if (GetAsyncKeyState('W') & 0x8000)
    {
        ///
        Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 0.1f);
        Pawn->GetRootComponent()->ComponentVelocity += Pawn->GetActorForwardVector() * 0.1f;
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() - Pawn->GetActorForwardVector() * 0.1f);
        Pawn->GetRootComponent()->ComponentVelocity += -Pawn->GetActorForwardVector() * 0.1f;

    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() - Pawn->GetActorRightVector() * 0.1f);
        Pawn->GetRootComponent()->ComponentVelocity += -Pawn->GetActorRightVector() * 0.1f;

    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorRightVector() * 0.1f);
        Pawn->GetRootComponent()->ComponentVelocity += Pawn->GetActorRightVector() * 0.1f;
    }
    POINT cur;
    GetCursorPos(&cur);

    float dx = (cur.x - MouseCenterPos.x) * MouseSens;
    float dy = (cur.y - MouseCenterPos.y) * MouseSens;

    SetCursorPos(MouseCenterPos.x, MouseCenterPos.y);

    FRotator ControlRot = Pawn->GetActorRotation();
    ControlRot.Yaw = std::fmod(ControlRot.Yaw + dx, 360.f);
    // ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch + dy, -45.f, +45.f);

    Pawn->GetRootComponent()->SetRelativeRotation(ControlRot);
}

void APlayerController::BeginPlay()
{
    AController::BeginPlay();

    InitMouseLook();
}

void APlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AController::EndPlay(EndPlayReason);

    //MyMessageHandler->OnKeyDownDelegate.RemoveAllForObject(this);
    //MyMessageHandler->OnKeyUpDelegate.RemoveAllForObject(this);

    //MyMessageHandler->OnMouseMoveDelegate.RemoveAllForObject(this);
}

void APlayerController::Initialize()
{
    SpawnPlayerCameraManager();

    MyMessageHandler = GEngineLoop.GetAppMessageHandler();
    if (MyMessageHandler)
    {
        MyMessageHandler->OnKeyDownDelegate.AddDynamic(this, &APlayerController::HandleKeyDown);
        MyMessageHandler->OnKeyUpDelegate.AddDynamic(this, &APlayerController::HandleKeyUp);

        //MyMessageHandler->OnMouseMoveDelegate.AddDynamic(this, &APlayerController::HandleMouseMove);
    }
}

void APlayerController::InitMouseLook()
{
    FWindowsCursor::SetShowMouseCursor(false);

    // 2) 화면 중앙 계산 (클라이언트 영역)
    
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    D3D11_VIEWPORT Viewport =GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->GetD3DViewport();
    MouseCenterPos.x = Viewport.TopLeftX + Viewport.Width / 2;
    MouseCenterPos.y = Viewport.TopLeftY + Viewport.Height / 2;

    // 3) 커서를 중앙으로 옮기기
    SetCursorPos(MouseCenterPos.x, MouseCenterPos.y);
}

void APlayerController::SpawnPlayerCameraManager()
{
    PlayerCameraManager = GetWorld()->SpawnActor<APlayerCameraManager>();
    PlayerCameraManager->InitializeFor(this);
    if (Pawn)
        PlayerCameraManager->SetViewTarget(Pawn);
    else
    {
        UE_LOG(ELogLevel::Error, "PlayerController dont have any Posses pawn");
    }

    CameraShakeModifier = FObjectFactory::ConstructObject<UCameraShakeBase>(this);
    if (CameraShakeModifier)
    {
        PlayerCameraManager->AddCameraModifier(CameraShakeModifier);
    }
    else
    {
        UE_LOG(ELogLevel::Error, "Failed to construct CameraShakeBase");
    }
}

void APlayerController::HandleKeyDown(const FKeyEvent& InKeyEvent)
{
    EInputEvent InputEvent = InKeyEvent.GetInputEvent();
    if (InputEvent == IE_Pressed)
    {
        //EKeys::Type KeyType = InKeyEvent.GetKey();
        uint32 character = InKeyEvent.GetCharacter();

        switch (character)
        {
        case 'W':
            bIsRunning = true;
            break;
        case 'A':
            bIsRunning = true;
            break;
        case 'S':
            bIsRunning = true;
            break;
        case 'D':
            bIsRunning = true;
            break;
        default:
            break;
        }
    }
    if (!bIsRunning) return;

    if (this->CameraShakeModifier != nullptr)
    {
        this->CameraShakeModifier->PlayShake();

        UE_LOG(ELogLevel::Display, "PlayShake");
    }
    else
    {
        UE_LOG(ELogLevel::Error, TEXT("CameraShakeModifier is NULL when trying to PlayShake!"));
    }
}

void APlayerController::HandleKeyUp(const FKeyEvent& InKeyEvent)
{
    EInputEvent InputEvent = InKeyEvent.GetInputEvent();
    if (InputEvent == IE_Released)
    {
        //EKeys::Type KeyType = InKeyEvent.GetKey();
        uint32 character = InKeyEvent.GetCharacter();
        switch (character)
        {
        case 'W':
            bIsRunning = false;
            break;
        case 'A':
            bIsRunning = false;
            break;
        case 'S':
            bIsRunning = false;
            break;
        case 'D':
            bIsRunning = false;
            break;
        default:
            break;
        }
    }
    if (bIsRunning) return;

    if (this->CameraShakeModifier != nullptr)
    {
        this->CameraShakeModifier->StopShake();
        UE_LOG(ELogLevel::Display, "StopShake");
    }
    else
    {
        UE_LOG(ELogLevel::Error, TEXT("CameraShakeModifier is NULL when trying to PlayShake!"));
    }
}

//void APlayerController::HandleMouseMove(const FPointerEvent& InPointerEvent)
//{
//}


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
APlayerController::APlayerController()
{

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

void APlayerController::Initialize()
{
    SpawnPlayerCameraManager();
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
    GEngineLoop.LuaCompiler.AddPlayerCameraMangerToLua(PlayerCameraManager);
    PlayerCameraManager->InitializeFor(this);

    UCameraFadeInModifier* CameraModifier = FObjectFactory::ConstructObject<UCameraFadeInModifier>(this);
    CameraModifier->Initialize(FLinearColor::Red, 5.0f);
    PlayerCameraManager->AddCameraModifier(CameraModifier);
}

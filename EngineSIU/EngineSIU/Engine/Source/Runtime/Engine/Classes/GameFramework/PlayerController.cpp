#include "PlayerController.h"

#include "Pawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/EditorEngine.h"
#include "Engine/Engine.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"

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

    if (GetAsyncKeyState('W') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 0.1f);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() - Pawn->GetActorForwardVector() * 0.1f);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() - Pawn->GetActorRightVector() * 0.1f);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorRightVector() * 0.1f);
    }
    POINT cur;
    GetCursorPos(&cur);
    
    // 4) 중앙 대비 델타 계산
    float dx = (cur.x - MouseCenterPos.x) * MouseSens;
    float dy = (cur.y - MouseCenterPos.y) * MouseSens;

    // 5) 커서 중앙으로 리셋 (끝에 화면 경계 문제 방지)
    SetCursorPos(MouseCenterPos.x, MouseCenterPos.y);

    // ─────────────────────────────────────────────────
    // 6) 컨트롤러 회전 업데이트
    FRotator ControlRot = Pawn->GetActorRotation();
    ControlRot.Yaw   = std::fmod(ControlRot.Yaw + dx, 360.f);
    // ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch + dy, -45.f, +45.f);
    // TSet<UActorComponent*> Components = Pawn->GetComponents();
    // for (auto Component : Components)
    // {
    //     UCameraComponent* CamComp = dynamic_cast<UCameraComponent*>(Component);
    //     if (CamComp)
    //     {
    //         FRotator CamRot =CamComp->GetRelativeRotation();
    //         // CamRot.Yaw += std::fmod(CamRot.Yaw + dx, 360.f);;
    //         // CamRot.Pitch = FMath::Clamp(CamRot.Pitch + dy, -45.f, +45.f);
    //         CamComp->SetRelativeRotation(CamRot);
    //     }
    // }
    // ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch + dy, -45.f, +45.f);
    Pawn->GetRootComponent()->SetRelativeRotation(ControlRot);

    // 7) 폰은 Yaw만 따라 돌리기 (3인칭용)
    // if (APawn* P = GetPawn())
    // {
    //     FRotator PawnRot = P->GetActorRotation();
    //     PawnRot.Yaw = ControlRot.Yaw;
    //     P->SetActorRotation(PawnRot);
    // }
}

void APlayerController::BeginPlay()
{
    AController::BeginPlay();

    InitMouseLook();
}

void APlayerController::InitMouseLook()
{
    // ShowCursor(false);
    SetCursor(NULL);

    // 2) 화면 중앙 계산 (클라이언트 영역)
    
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    D3D11_VIEWPORT Viewport =GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->GetD3DViewport();
    MouseCenterPos.x = Viewport.TopLeftX + Viewport.Width / 2;
    MouseCenterPos.y = Viewport.TopLeftY + Viewport.Height / 2;

    // 3) 커서를 중앙으로 옮기기
    SetCursorPos(MouseCenterPos.x, MouseCenterPos.y);
}

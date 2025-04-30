#include "PlayerCameraManager.h"

#include "CameraComponent.h"
#include "CameraModifier.h"
#include "GameFramework/PlayerController.h"

APlayerCameraManager::APlayerCameraManager()
{
    TransformComponent = AddComponent<USceneComponent>("TransformComponent_0");
    RootComponent = TransformComponent;
}

APlayerCameraManager::~APlayerCameraManager()
{
}

void APlayerCameraManager::InitializeFor(APlayerController* PC)
{
    PCOwner = PC;
}

void APlayerCameraManager::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

    UCameraComponent* Camera =nullptr;
    for (auto Component : ViewTarget.Target->GetComponents())
    {
        if (Cast<UCameraComponent>(Component) != nullptr)
        {
            Camera = Cast<UCameraComponent>(Component);
        }
    }

    if (!Camera) return;

    // 현재 카메라 상태 가져오기 (Modifier 적용 전)
    FVector CurrentLocation = Camera->GetRelativeLocation(); // 또는 GetWorldLocation() - 일관성 중요
    FRotator CurrentRotation = Camera->GetRelativeRotation(); // 또는 GetWorldRotation()
    float CurrentFOV = Camera->GetFieldOfView();

    // Modifier 적용을 위한 임시 변수 (현재 상태로 초기화)
    FVector ModifiedLocation = CurrentLocation;
    FRotator ModifiedRotation = CurrentRotation;
    float ModifiedFOV = CurrentFOV;

    // 모든 활성화된 Modifier 순회 및 적용
    for (auto modifier : ModifierList)
    {
        if (modifier && !modifier->IsDisabled()) // nullptr 체크 추가
        {
            // 이전 Modifier의 결과(Modified...)를 현재 Modifier의 입력(Current...)으로 사용
            modifier->ModifyCamera(
                DeltaTime,
                ModifiedLocation,  // 이전 단계의 결과 위치
                ModifiedRotation,  // 이전 단계의 결과 회전
                ModifiedFOV,       // 이전 단계의 결과 FOV
                ModifiedLocation,  // 수정될 새 위치 (In/Out)
                ModifiedRotation,  // 수정될 새 회전 (In/Out)
                ModifiedFOV        // 수정될 새 FOV (In/Out)
            );
            // ModifyCamera 함수는 입력값을 기반으로 출력 파라미터를 수정해야 함
        }
    }
    // 최종적으로 계산된 값으로 카메라 설정
    Camera->SetRelativeLocation(ModifiedLocation); // 또는 SetWorldLocation()
    Camera->SetRelativeRotation(ModifiedRotation); // 또는 SetWorldRotation()
    Camera->SetFieldOfView(ModifiedFOV);


}

void APlayerCameraManager::AddCameraModifier(UCameraModifier* CameraModifier)
{
    //ModifierList.AddUnique(CameraModifier);

    CameraModifier->AddedToCamera(this);
    ModifierList.AddUnique(CameraModifier);
}

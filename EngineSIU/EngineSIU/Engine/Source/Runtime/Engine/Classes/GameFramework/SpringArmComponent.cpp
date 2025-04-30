#include "SpringArmComponent.h"

#include "Pawn.h"
#include "UObject/UObjectIterator.h"
#include "Runtime/InteractiveToolsFramework/BaseGizmos/GizmoBaseComponent.h"
#include "Engine/Engine.h"
#include "Engine/EditorEngine.h"
#include "World/World.h"

UObject* USpringArmComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    // SpringArm 고유 프로퍼티 복사
    NewComponent->TargetArmLength = TargetArmLength;
    NewComponent->SocketOffset = SocketOffset;
    NewComponent->TargetOffset = TargetOffset;
    NewComponent->bDoCollisionTest = bDoCollisionTest;
    NewComponent->bUsePawnControlRotation = bUsePawnControlRotation;
    NewComponent->bInheritPitch = bInheritPitch;
    NewComponent->bInheritYaw = bInheritYaw;
    NewComponent->bInheritRoll = bInheritRoll;
    NewComponent->bEnableCameraLag = bEnableCameraLag;
    NewComponent->CameraLagSpeed = CameraLagSpeed;
    NewComponent->CameraLagMaxDistance = CameraLagMaxDistance;
    NewComponent->bEnableCameraRotationLag = bEnableCameraRotationLag;
    NewComponent->CameraRotationLagSpeed = CameraRotationLagSpeed;

    // 런타임 정보(PreviousDesiredLoc 등)는 복사할 필요 없음 (초기화 상태로 두는 것이 일반적)

    return NewComponent;
}

void USpringArmComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);

    OutProperties.Add(TEXT("TargetArmLength"), FString::SanitizeFloat(TargetArmLength));
    OutProperties.Add(TEXT("SocketOffset"), SocketOffset.ToString());
    OutProperties.Add(TEXT("TargetOffset"), TargetOffset.ToString());
    OutProperties.Add(TEXT("bDoCollisionTest"), bDoCollisionTest ? TEXT("true") : TEXT("false"));
    OutProperties.Add(TEXT("bUsePawnControlRotation"), bUsePawnControlRotation ? TEXT("true") : TEXT("false"));
    OutProperties.Add(TEXT("bInheritPitch"), bInheritPitch ? TEXT("true") : TEXT("false"));
    OutProperties.Add(TEXT("bInheritYaw"), bInheritYaw ? TEXT("true") : TEXT("false"));
    OutProperties.Add(TEXT("bInheritRoll"), bInheritRoll ? TEXT("true") : TEXT("false"));
    OutProperties.Add(TEXT("bEnableCameraLag"), bEnableCameraLag ? TEXT("true") : TEXT("false"));
    OutProperties.Add(TEXT("CameraLagSpeed"), FString::SanitizeFloat(CameraLagSpeed));
    OutProperties.Add(TEXT("CameraLagMaxDistance"), FString::SanitizeFloat(CameraLagMaxDistance));
    OutProperties.Add(TEXT("bEnableCameraRotationLag"), bEnableCameraRotationLag ? TEXT("true") : TEXT("false"));
    OutProperties.Add(TEXT("CameraRotationLagSpeed"), FString::SanitizeFloat(CameraRotationLagSpeed));
}

void USpringArmComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);

    auto GetBoolProp = [&](const FString& Key, bool& OutVal) {
        const FString* Str = InProperties.Find(Key);
        if (Str) OutVal = (*Str == TEXT("true"));
        };

    auto GetFloatProp = [&](const FString& Key, float& OutVal) {
        const FString* Str = InProperties.Find(Key);
        if (Str) OutVal = FCString::Atof(**Str);
        };

    auto GetVectorProp = [&](const FString& Key, FVector& OutVal) {
        const FString* Str = InProperties.Find(Key);
        if (Str) OutVal.InitFromString(*Str);
        };

    GetFloatProp(TEXT("TargetArmLength"), TargetArmLength);
    GetVectorProp(TEXT("SocketOffset"), SocketOffset);
    GetVectorProp(TEXT("TargetOffset"), TargetOffset);
    GetBoolProp(TEXT("bDoCollisionTest"), bDoCollisionTest);
    GetBoolProp(TEXT("bUsePawnControlRotation"), bUsePawnControlRotation);
    GetBoolProp(TEXT("bInheritPitch"), bInheritPitch);
    GetBoolProp(TEXT("bInheritYaw"), bInheritYaw);
    GetBoolProp(TEXT("bInheritRoll"), bInheritRoll);
    GetBoolProp(TEXT("bEnableCameraLag"), bEnableCameraLag);
    GetFloatProp(TEXT("CameraLagSpeed"), CameraLagSpeed);
    GetFloatProp(TEXT("CameraLagMaxDistance"), CameraLagMaxDistance);
    GetBoolProp(TEXT("bEnableCameraRotationLag"), bEnableCameraRotationLag);
    GetFloatProp(TEXT("CameraRotationLagSpeed"), CameraRotationLagSpeed);
}

USpringArmComponent::USpringArmComponent()
{
    TargetArmLength = 5.0f;
    SocketOffset = FVector(0.f, 0.f, 0.f);
    TargetOffset = FVector(0.f, 0.f, 0.f);
    bDoCollisionTest = true;
    bUsePawnControlRotation = true;
    bInheritPitch = true;
    bInheritYaw = true;
    bInheritRoll = true;
    bEnableCameraLag = false;
    CameraLagSpeed = 1.0f;
    CameraLagMaxDistance = 0.0f;
    bEnableCameraRotationLag = false;
    CameraRotationLagSpeed = 1.0f;
    PreviousDesiredLoc = FVector::ZeroVector;
    PreviousArmOrigin = FVector::ZeroVector;
    PreviousDesiredRot = FRotator::ZeroRotator;
    bIsCameraFixed = false;
    UnfixedCameraPosition = FVector::ZeroVector;
}

FRotator USpringArmComponent::GetDesiredRotation() const
{
    return GetWorldRotation();
}

FRotator USpringArmComponent::GetTargetRotation() const
{
    FRotator DesiredRot = GetDesiredRotation();

    if (bUsePawnControlRotation)
    {
        if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
        {
            const FRotator PawnViewRotation = OwningPawn->GetActorRotation();
            if (DesiredRot != PawnViewRotation)
            {
                DesiredRot = PawnViewRotation;
            }
        }
    }

    const FRotator LocalRelativeRotation = GetRelativeRotation();
    if (!bInheritPitch)
    {
        DesiredRot.Pitch = LocalRelativeRotation.Pitch;
    }

    if (!bInheritYaw)
    {
        DesiredRot.Yaw = LocalRelativeRotation.Yaw;
    }

    if (!bInheritRoll)
    {
        DesiredRot.Roll = LocalRelativeRotation.Roll;
    }

    return DesiredRot;
}

void USpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
    FRotator DesiredRot = GetTargetRotation();
    if (bDoRotationLag)
    {
        DesiredRot = FRotator(FMath::QInterpTo(PreviousDesiredRot.ToQuaternion(), DesiredRot.ToQuaternion(), DeltaTime, CameraRotationLagSpeed));
    }
    PreviousDesiredRot = DesiredRot;

    FVector ArmOrigin = GetWorldLocation() + TargetOffset;
    FVector DesiredLoc = ArmOrigin;
    if (bDoLocationLag)
    {
        DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
    }
    bool bClampedDist = false;
    if (CameraLagMaxDistance > 0.f)
    {
        const FVector FromOrigin = DesiredLoc - ArmOrigin;
        if (FromOrigin.LengthSquared() > FMath::Square(CameraLagMaxDistance))
        {
            DesiredLoc = ArmOrigin + FromOrigin.GetClampedToMaxSize(CameraLagMaxDistance);
            bClampedDist = true;
        }
    }
    PreviousArmOrigin = ArmOrigin;
    PreviousDesiredLoc = DesiredLoc;

    // Now offset camera position back along our rotation
    const float CP = FMath::Cos(FMath::DegreesToRadians(DesiredRot.Pitch));
    
    DesiredLoc -= FVector(
        FMath::Cos(FMath::DegreesToRadians(DesiredRot.Yaw)) * CP,
        FMath::Sin(FMath::DegreesToRadians(DesiredRot.Yaw)) * CP,
        FMath::Sin(FMath::DegreesToRadians(DesiredRot.Pitch))) * TargetArmLength;
    // Add socket offset in local space
    FVector4 Vector4 = DesiredRot.ToMatrix().TransformFVector4(FVector4(SocketOffset, 0.f));
    DesiredLoc += FVector(Vector4.X, Vector4.Y, Vector4.Z);

    FVector ResultLoc;
    if (bDoTrace && (TargetArmLength != 0.0f))
    {
        bIsCameraFixed = true;

        USceneComponent* Possible = nullptr;
        int maxIntersect = 0;
        float minDistance = FLT_MAX;
        FVector Direction = (DesiredLoc - ArmOrigin).GetSafeNormal();

        AActor* Parent = GetOwner();
        TSet<UActorComponent*> ParentComponents = Parent->GetComponents();
        UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
        for (AActor* Actor : Engine->ActiveWorld->GetActiveLevel()->Actors)
        {
            TSet<UActorComponent*> ActorComponents = Actor->GetComponents();
            for (UActorComponent* Comp : ActorComponents)
            {
                UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Comp);
                if (!PrimComp) continue;
                if (ParentComponents.Contains(PrimComp)) continue;
                if (PrimComp->IsA<UGizmoBaseComponent>()) continue;

                FMatrix InverseWorld = FMatrix::Inverse(PrimComp->GetWorldMatrix());
                FVector LocalRayOrigin = InverseWorld.TransformPosition(ArmOrigin);
                FVector LocalRayDir = InverseWorld.TransformVector(Direction, InverseWorld);

                float Distance = 0.0f;
                int currentIntersectCount = 0;
                if (currentIntersectCount = PrimComp->CheckRayIntersection(LocalRayOrigin, LocalRayDir, Distance))
                {
                    if (Distance < TargetArmLength)
                    {
                        if (Distance < minDistance)
                        {
                            minDistance = Distance;
                            maxIntersect = currentIntersectCount;
                            Possible = PrimComp;
                        }
                        else if (abs(Distance - minDistance) < FLT_EPSILON && currentIntersectCount > maxIntersect)
                        {
                            maxIntersect = currentIntersectCount;
                            Possible = PrimComp;
                        }
                    }
                }
            }
        }

        FVector HitLocation = ArmOrigin + Direction * minDistance;
        UnfixedCameraPosition = DesiredLoc;
        bool bBlockingHit = Possible != nullptr;
        ResultLoc = BlendLocations(DesiredLoc, HitLocation, bBlockingHit, DeltaTime);

        if (ResultLoc == DesiredLoc)
        {
            bIsCameraFixed = false;
        }
    }
    else
    {
        ResultLoc = DesiredLoc;
        bIsCameraFixed = false;
        UnfixedCameraPosition = ResultLoc;
    }

    FVector ParentLocation = GetWorldLocation();
    FQuat ParentRotation = GetWorldRotation().ToQuaternion();
    FVector ParentScale = GetWorldScale3D();

    FVector CameraWorldLocation = ResultLoc;
    FQuat CameraWorldRotation = DesiredRot.ToQuaternion();

    // 상대 위치 = (카메라 위치 - 부모 위치), 부모 회전/스케일의 역변환 적용
    FVector RelativeLocation = ParentRotation.Inverse().RotateVector((CameraWorldLocation - ParentLocation) / ParentScale);

    // 상대 회전 = 부모 회전의 역 * 카메라 회전
    FQuat RelativeRotation = ParentRotation.Inverse() * CameraWorldRotation;

    RelativeSocketLocation = RelativeLocation;
    RelativeSocketRotation = RelativeRotation;

    // 자식 컴포넌트의 상대 위치/회전 갱신
    for (USceneComponent* ChildComp : GetAttachChildren())
    {
        if (ChildComp)
        {
            ChildComp->SetRelativeLocation(RelativeSocketLocation);
            ChildComp->SetRelativeRotation(RelativeSocketRotation.ToRotator());
        }
    }
}

void USpringArmComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
    UpdateDesiredArmLocation(bDoCollisionTest, bEnableCameraLag, bEnableCameraRotationLag, DeltaTime);
}

FVector USpringArmComponent::BlendLocations(const FVector& DesiredArmLocation, const FVector& TraceHitLocation, bool bHitSomething, float DeltaTime)
{
    static FVector PreviousCameraLocation = DesiredArmLocation;
    FVector Result;
    if (bHitSomething)
    {
        Result = FMath::VInterpTo(PreviousCameraLocation, TraceHitLocation, DeltaTime, CameraLagSpeed);
    }
    else
    {
        Result = DesiredArmLocation;
    }
    PreviousCameraLocation = Result;
    return Result;
}

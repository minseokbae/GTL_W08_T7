#include "CameraLetterBoxIn.h"

#include "PlayerCameraManager.h"
#include "Math/JungleMath.h"

void UCameraLetterBoxIn::Initialize(float Duration)
{
    TargetDuration = Duration;
}

bool UCameraLetterBoxIn::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    Start -= DeltaTime/TargetDuration;
    NewCameraManager->LetterBoxHeight = Start;
    NewCameraManager->LetterBoxWidth =  Start;
    if (NewCameraManager->LetterBoxHeight <= 0.0f)
        return true;
    return false;;
}

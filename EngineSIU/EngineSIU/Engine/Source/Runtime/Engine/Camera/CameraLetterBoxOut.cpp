#include "CameraLetterBoxOut.h"
#include "PlayerCameraManager.h"
bool UCameraLetterBoxOut::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    Start += DeltaTime/TargetDuration;
    NewCameraManager->LetterBoxHeight = Start;
    NewCameraManager->LetterBoxWidth =  Start;
    if (NewCameraManager->LetterBoxHeight >= 5.0f)
        return true;
    return false;;
}

void UCameraLetterBoxOut::Initialize(float Duration)
{
    TargetDuration = Duration;
}

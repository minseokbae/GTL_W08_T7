#include "GameFramework/Actor.h"


#if 1 // TODO: WITH_EDITOR 추가

FString AActor::GetDefaultActorLabel() const
{
    const UClass* ActorClass = GetClass();
    FString DefaultActorLabel = ActorClass->GetName();
    return DefaultActorLabel;
}

FString AActor::GetActorLabel() const
{
    if (ActorLabel.IsEmpty())
    {
        FString DefaultActorLabel = GetDefaultActorLabel();
        // TODO: Unique한 이름으로 만들기
        DefaultActorLabel += FString::FromInt(GetUUID());

        AActor* MutableThis = const_cast<AActor*>(this);
        MutableThis->ActorLabel = std::move(DefaultActorLabel);
    }

    return ActorLabel; 
}

void AActor::SetActorLabel(const FString& NewActorLabel, bool bUUID)
{
    // NewActorLabel != GetActorLabel
    if (FCString::Strcmp(*NewActorLabel, *GetActorLabel()) != 0)
    {
        if (bUUID == true)
        {
            
            ActorLabel = NewActorLabel + "_" + FString::FromInt(GetUUID());
        }
        else
        {
            ActorLabel = NewActorLabel;
        }
    }
    
}

void AActor::SetLuaBindState(bool state)
{
    bHasLua = state;
}

void AActor::SetLuaScriptPath(FString FilePath)
{
    LuaScriptPath = FilePath;
}

void AActor::SetActorTag(const FString& NewTag)
{
    if (!RootComponent)
        return;
    RootComponent->SetTag(NewTag);
}

FString AActor::GetActorTag()
{
    if (!RootComponent)
        return "";
    return RootComponent->GetTag();
}

#endif

#pragma once
#include "Engine.h"
#include "Actors/EditorPlayer.h"

/*
    Editor 모드에서 사용될 엔진.
    UEngine을 상속받아 Editor에서 사용 될 기능 구현.
    내부적으로 PIE, Editor World 두 가지 형태로 관리.
*/

class UGameInstance;
class APlayerController;
class AGameMode;
class AActor;
class USceneComponent;

class UEditorEngine : public UEngine
{
    DECLARE_CLASS(UEditorEngine, UEngine)

public:
    UEditorEngine() = default;

    virtual void Init() override;
    virtual void Tick(float DeltaTime) override;
    void Input();
    bool bF8Clicked = false;
    UWorld* PIEWorld = nullptr;
    UWorld* EditorWorld = nullptr;

    void StartPIE();
    void EndPIE();

    //TODO : UGameEngine 으로 빠져야할 부부입니다. 
    void InitGame();
    
    // 주석은 UE에서 사용하던 매개변수.
    FWorldContext& GetEditorWorldContext(/*bool bEnsureIsGWorld = false*/);
    FWorldContext* GetPIEWorldContext(/*int32 WorldPIEInstance = 0*/);

public:
    void SelectActor(AActor* InActor) const;
    bool CanSelectActor(const AActor* InActor) const;
    AActor* GetSelectedActor() const;

    void HoverActor(AActor* InActor) const;

    
    void NewWorld();

    void SelectComponent(USceneComponent* InComponent) const;
    bool CanSelectComponent(const USceneComponent* InComponent) const;
    USceneComponent* GetSelectedComponent() const;

    void HoverComponent(USceneComponent* InComponent) const;

public:
    AEditorPlayer* GetEditorPlayer() const;
    AController* GetCurrentController() {return CurrentPlayer;}
private:
    AEditorPlayer* EditorPlayer = nullptr;
    AController* CurrentPlayer = nullptr;
    APlayerController* PlayerController = nullptr;
    AGameMode* GameMode = nullptr;
    UGameInstance* GameInstance = nullptr;
};




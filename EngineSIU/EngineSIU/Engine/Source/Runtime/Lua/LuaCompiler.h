#pragma once
#include "LuaInstance.h"
#include "HAL/PlatformType.h"
#include <memory>

class AActor;

class UCameraModifier;

class UCameraTransitionModifier;

class APlayerCameraManager;

class FLuaCompiler
{
public:
    FLuaCompiler();

    void Bind(AActor* Actor);

    void UnBind(AActor* Actor);

    void BeginPlay();

    void EndPlay();

    void Tick(float DeltaTime);

    void UpdateInput();

    void AddPlayerCameraMangerToLua(APlayerCameraManager* CameraManager);

    void UpdateGlobal(std::unique_ptr<FLuaInstance> LuaInstance);

private:
    sol::state Lua;
    sol::table Input;
    std::unordered_map<uint32, std::unique_ptr<FLuaInstance>> LuaInstances;
    std::filesystem::file_time_type LastWriteTime;

    void AddScore(float score);
    void GameOver();

    UCameraTransitionModifier* CreateCameraTransitionModifier(USceneComponent* Comp);
    void ChangeViewMode(int ViewModeIndex);
};

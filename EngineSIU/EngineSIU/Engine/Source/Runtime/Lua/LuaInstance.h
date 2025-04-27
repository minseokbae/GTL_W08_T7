#pragma once
#define SOL_LUAJIT 1
#include <sol/sol.hpp>
#include <lua.hpp>
#include "Container/String.h"
#include <filesystem>
#include "Delegates/Delegate.h"

class AActor;

class UShapeComponent;

class USceneComponent;

class FLuaInstance
{
private:
    sol::environment Env;
    sol::function TickFunc;
    sol::function BeginPlayFunc;
    sol::function EndPlayFunc;
    sol::function OnOverlapFunc;
    sol::function OnEndOverlapFunc;
    std::string ScriptFile;
    std::filesystem::file_time_type LastWriteTime;
    AActor* BindedActor = nullptr;
    UShapeComponent* ShapeComp = nullptr;

    FDelegateHandle BeginOverlapHandle;
    FDelegateHandle EndOverlapHandle;
public:
    FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath);
    std::string GetScriptFile() { return ScriptFile; }
    std::filesystem::file_time_type GetLastWriteTime() { return LastWriteTime; }
    void Tick(float DeltaTime);
    void BeginPlay();
    void EndPlay();
    void OnOverlap(USceneComponent* OverlappedComp);
    void Reload(sol::state& Lua);
    AActor* GetBindedActor() { return BindedActor; }
    void BindDelegates();
    void UnBindDelegates();
    void OnComponentBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
    void OnComponentEndOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
};

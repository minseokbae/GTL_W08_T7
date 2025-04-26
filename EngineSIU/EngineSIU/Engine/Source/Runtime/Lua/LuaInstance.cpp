#include "LuaInstance.h"
#include "Classes/Components/SceneComponent.h"
#include "GameFramework/Actor.h"

FLuaInstance::FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath)
    : Env(Lua, sol::create, Lua.globals())
{
    Env["obj"] = Comp;

    BindedActor = Comp->GetOwner();

    // Load script
    ScriptFile = *FilePath;

    Lua.script_file(ScriptFile, Env);

    TickFunc = Env["Tick"];
    BeginPlayFunc = Env["BeginPlay"];
    EndPlayFunc = Env["EndPlay"];
    LastWriteTime = std::filesystem::last_write_time(ScriptFile);
}

void FLuaInstance::Tick(float DeltaTime)
{
    if (TickFunc.valid()) 
    {
        TickFunc(DeltaTime);
    }
}

void FLuaInstance::BeginPlay()
{
    if (BeginPlayFunc.valid())
    {
        BeginPlayFunc();
    }
}

void FLuaInstance::EndPlay()
{
    if (EndPlayFunc.valid())
    {
        EndPlayFunc();
    }
}

void FLuaInstance::Reload(sol::state& Lua) {
    Lua.script_file(ScriptFile, Env);
    TickFunc = Env["Tick"];
    BeginPlayFunc = Env["BeginPlay"];
    EndPlayFunc = Env["EndPlay"];
    LastWriteTime = std::filesystem::last_write_time(ScriptFile);
}

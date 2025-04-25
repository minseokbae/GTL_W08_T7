#include "LuaInstance.h"
#include "Classes/Components/SceneComponent.h"

FLuaInstance::FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath)
    : Env(Lua, sol::create, Lua.globals())
{
    Env["obj"] = Comp;

    // Load script
    ScriptFile = *FilePath;

    Lua.script_file(ScriptFile, Env);

    TickFunc = Env["Tick"];
    LastWriteTime = std::filesystem::last_write_time(ScriptFile);
}

void FLuaInstance::Tick(float DeltaTime)
{
    if (TickFunc.valid()) {
        TickFunc(DeltaTime);
    }
}

void FLuaInstance::Reload(sol::state& Lua) {
    Lua.script_file(ScriptFile, Env);
    TickFunc = Env["Tick"];
    LastWriteTime = std::filesystem::last_write_time(ScriptFile);
}

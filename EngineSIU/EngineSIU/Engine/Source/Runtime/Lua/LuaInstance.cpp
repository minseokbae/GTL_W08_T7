#include "LuaInstance.h"
#include "Classes/Components/SceneComponent.h"

FLuaInstance::FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath)
    : Env(Lua, sol::create, Lua.globals())
{
    Env["obj"] = Comp;

    // Load script
    std::string ansiStr = *FilePath;

    Lua.script_file(ansiStr, Env);

    TickFunc = Env["Tick"];
}

void FLuaInstance::Tick(float DeltaTime)
{
    if (TickFunc.valid()) {
        TickFunc(DeltaTime);
    }
}

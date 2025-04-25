#include "LuaInstance.h"
#include "Classes/Components/SceneComponent.h"

FLuaInstance::FLuaInstance(sol::state& Lua, USceneComponent* Comp)
    : Env(Lua, sol::create, Lua.globals())
{
    Env["obj"] = Comp;

    // Load script
    Lua.script_file("template.lua", Env);

    TickFunc = Env["Tick"];
}

void FLuaInstance::Tick(float DeltaTime)
{
    if (TickFunc.valid()) {
        TickFunc(DeltaTime);
    }
}
